// connectionlistmodel.cpp
//
// Qt model for network connections.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of
//   the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <QProcess>

#include "connectionlistmodel.h"
#include "paths.h"
#include "profile.h"

#define CONNECTIONLISTMODEL_ROW_HEIGHT 24

Connection::Connection(int pid,const QHostAddress &addr,int port)
{
  d_pid=pid;
  d_address=addr;
  d_port=port;
}


Connection::Connection()
{
  d_pid=0;
  d_port=0;
}


bool Connection::isValid() const
{
  return d_pid>0;
}


int Connection::pid() const
{
  return d_pid;
}


QHostAddress Connection::address() const
{
  return d_address;
}


int Connection::port() const
{
  return d_port;
}


QString Connection::hostname() const
{
  return d_hostname;
}


void Connection::setHostname(const QString &str)
{
  d_hostname=str;
}


QString Connection::idString() const
{
  if(d_hostname.isEmpty()||(d_hostname==d_address.toString())) {
    return d_address.toString()+QString::asprintf(":%d",d_port);
  }
  return d_hostname+QString::asprintf(":%d [",d_port)+
    d_address.toString()+"]";
}


Connection Connection::operator=(const Connection &rhs)
{
  Connection ret(rhs.d_pid,rhs.d_address,rhs.d_port);
  return ret;
}


bool Connection::operator==(const Connection &other) const
{
  return (d_pid==other.d_pid)&&
    (d_address==other.d_address)&&
    (d_port==other.d_port);
}




ConnectionListModel::ConnectionListModel(QWidget *parent)
  : QAbstractListModel((QObject *)parent)
{
  d_scan_process=NULL;
  d_font_metrics=NULL;

  d_proc_dir=new QDir("/proc");

  d_scan_timer=new QTimer(this);
  d_scan_timer->setSingleShot(true);
  connect(d_scan_timer,SIGNAL(timeout()),this,SLOT(scanData()));
  d_scan_timer->start(0);
}


ConnectionListModel::~ConnectionListModel()
{
  delete d_font_metrics;
}


Connection ConnectionListModel::connection(const QModelIndex &index) const
{
  if(index.isValid()) {
    return d_connections.at(index.row());
  }
  return Connection();
}


void ConnectionListModel::setFont(const QFont &font)
{
  d_font_metrics=new QFontMetrics(font);
}


int ConnectionListModel::rowCount(const QModelIndex &parent) const
{
  return d_connections.size();
}


QVariant ConnectionListModel::data(const QModelIndex &index,int role) const
{
  int row=index.row();
  
  switch(role) {
  case Qt::DisplayRole:
    return d_connections.at(row).idString();

  case Qt::SizeHintRole:
    return QSize(d_font_metrics->
		 horizontalAdvance(d_connections.at(row).idString()),
		 CONNECTIONLISTMODEL_ROW_HEIGHT);
  }

  return QVariant();
}


QVariant ConnectionListModel::headerData(int section,Qt::Orientation orient,
					 int role) const
{
  switch(role) {
  case Qt::DisplayRole:
    return tr("Host");
  }

  return QVariant();
}


void ConnectionListModel::scanData()
{
  UpdateConnections();

  d_scan_timer->start(5000);
}


void ConnectionListModel::hostLookupFinished(const QHostInfo &hinfo)
{
  QList<QHostAddress> addrs=hinfo.addresses();
  for(int i=0;i<d_connections.size();i++) {
    if(addrs.contains(d_connections.at(i).address())) {
      d_connections[i].setHostname(hinfo.hostName());
      emit dataChanged(createIndex(i,0),createIndex(i,0));
    }
  }
}


void ConnectionListModel::UpdateConnections()
{
  //
  // Update Whitelist
  //
  Profile *p=new Profile();
  p->loadFile("/etc/vncmon.conf");
  p->loadFile(QDir::homePath()+"/.vnc/vncmonrc");
  QList<QHostAddress> whitelist=p->addressValues("Vncmon","Whitelist");
  delete p;

  //
  // Generate Current Connections List
  //
  QMap<int,Connection> conns;
  LoadConnectionTable(conns);

  //
  // Apply Whitelist
  //
  QList<int> ids;
  for(QMap<int,Connection>::const_iterator it=conns.begin();it!=conns.end();
      it++) {
    if(whitelist.contains(it.value().address())) {
      ids.push_back(it.key());
    }
  }
  for(int i=0;i<ids.size();i++) {
    conns.remove(ids.at(i));
  }

  //
  // Expire Removed Connections
  //
  for(int i=0;i<d_connections.size();i++) {
    if(!conns.contains(d_connections.at(i).pid())) {
      beginRemoveRows(QModelIndex(),i,i+1);
      d_connections.removeAt(i);
      i--;
      endRemoveRows();
    }
  }

  //
  // Add New Hosts
  //
  QList<Connection> new_conns;
  for(QMap<int,Connection>::const_iterator it=conns.begin();it!=conns.end();
      it++) {
    if((!d_connections.contains(it.value()))&&
       (!whitelist.contains(it.value().address()))) {
      new_conns.push_back(it.value());
      QHostInfo::lookupHost(it.value().address().toString(),
			    this,&ConnectionListModel::hostLookupFinished);
    }
  }
  if(new_conns.size()>0) {
    beginInsertRows(QModelIndex(),d_connections.size(),
		    d_connections.size()+new_conns.size());
    for(int i=0;i<new_conns.size();i++) {
      d_connections.push_back(new_conns.at(i));
    }
    endInsertRows();
  }

  UpdateGenmon(d_connections.size());
}


void ConnectionListModel::LoadConnectionTable(QMap<int,Connection> &conntab)
{
  FILE *f=NULL;
  char line[1024];
  bool ok=false;
  bool update_needed=false;
  
  //
  // Clear old table
  //
  conntab.clear();

  //
  // Load crontab
  //
  QString connpath=QDir::homePath()+"/.vnc/conntab";
  if((f=fopen(connpath.toUtf8(),"r"))!=NULL) {
    while(fgets(line,1023,f)!=NULL) {
      QStringList f0=QString::fromUtf8(line).split("\n",Qt::SkipEmptyParts);
      for(int i=0;i<f0.size();i++) {
	bool active=false;
	QStringList f1=f0.at(i).trimmed().split("\t",Qt::KeepEmptyParts);
	if(f1.size()==3) {
	  int pid=f1.at(0).toInt(&ok);
	  if(ok&&(pid>0)&&(d_proc_dir->exists(QString::asprintf("%d",pid)))) {
	    QHostAddress addr(f1.at(1));
	    if(!addr.isNull()) {
	      int port=f1.at(2).toInt(&ok);
	      if(ok&&(port>0)&&(port<=0xFFFF)) {
		conntab.insert(pid,Connection(pid,addr,port));
		active=true;
	      }
	    }
	  }
	}
	update_needed=update_needed||(!active);
      }
    }
    fclose(f);
  }

  //
  // Take out the trash
  //
  if(update_needed) {
    QString connpath_update=connpath+"-update";
    if((f=fopen(connpath_update.toUtf8(),"w"))!=NULL) {
      for(QMap<int,Connection>::const_iterator it=conntab.begin();
	  it!=conntab.end();it++) {
	fprintf(f,"%d\t%s\t%d\n",it.key(),
		it.value().address().toString().toUtf8().constData(),
		it.value().port());
      }
      fclose(f);
      rename(connpath_update.toUtf8(),connpath.toUtf8());
    }
  }
}


void ConnectionListModel::UpdateGenmon(int conn_quan)
{
  FILE *f=NULL;

  //
  // Generate the script
  //
  QString genmon_path=QDir::homePath()+"/.vnc/xfce4-genmon.sh";
  QString genmon_path_new=genmon_path+"-new";
  if((f=fopen(genmon_path_new.toUtf8(),"w"))!=NULL) {
    fprintf(f,"#!/bin/bash\n");
    fprintf(f,"\n");
    fprintf(f,"GENTIME=%llu\n",QDateTime::currentDateTime().toSecsSinceEpoch());
    fprintf(f,"NOW=`date +%%s`\n");
    fprintf(f,"DIFF=`bc <<<\"$NOW-$GENTIME\"`\n");
    fprintf(f,"if test $DIFF -gt 10 ; then\n");
    fprintf(f,"  %s/vncmon -display :0 &\n",BINDIR_PATH);
    fprintf(f,"  exit 0\n");
    fprintf(f,"fi\n");
    fprintf(f,"echo \"<click>kill -s SIGUSR1 %d</click>\"\n",getpid());
    switch(conn_quan) {
    case 0:
      fprintf(f,"echo \"<tool>No VNC connections</tool>\"\n");
      break;

    case 1:
      fprintf(f,"echo \"<tool>1 VNC connection</tool>\"\n");
      break;

    default:
      fprintf(f,"echo \"<tool>%d VNC connections</tool>\"\n",conn_quan);
      break;
    }
    if(conn_quan>6) {
      fprintf(f,"echo \"<img>/usr/share/x11vnc-auto/vncmon-many.png</img>\"\n");
    }
    else {
      fprintf(f,"echo \"<img>/usr/share/x11vnc-auto/vncmon-%d.png</img>\"\n",
	      conn_quan);
    }
    fclose(f);
    chmod(genmon_path_new.toUtf8(),S_IRUSR|S_IWUSR|S_IXUSR);
    rename(genmon_path_new.toUtf8(),genmon_path.toUtf8());
  }
  else {
    fprintf(stderr,"file open failed: %s\n",strerror(errno));
  }

  //
  // Refresh the plug-in
  //
  QStringList args;
  args.push_back("ax");
  QProcess *proc=new QProcess(this);
  proc->start("/bin/ps",args);
  proc->waitForFinished();
  QStringList f0=QString::fromUtf8(proc->readAllStandardOutput()).split("\n");
  delete proc;
  int offset;
  for(int i=0;i<f0.size();i++) {
    if((offset=f0.at(i).indexOf("libgenmon.so"))>0) {
      QString plugin_cmd=
	QString::asprintf("--plugin-event=genmon-%d:refresh:bool:true",
			  f0.at(i).mid(offset+12,4).toInt());
      args.clear();
      args.push_back(plugin_cmd);
      proc=new QProcess(this);
      proc->start("/usr/bin/xfce4-panel",args);
      proc->waitForFinished();
      delete proc;
    }
  }
}
