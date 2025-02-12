// vncmon.cpp
//
// Applet for monitoring VNC connections to a host
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QRect>
#include <QStringList>

#include "cmdswitch.h"
#include "vncmon.h"

//
// Icons
//
#include "../../icons/vncmon-16x16.xpm"

static int global_signal_sockets[2];

void SigHandler(int signum)
{
  char data[1]={0};

  write(global_signal_sockets[0],data,1);
}


MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  QString err_msg;
  CmdSwitch *cmd=new CmdSwitch("vncmon",VERSION,VNCMON_USAGE);
  for(int i=0;i<cmd->keys();i++) {
    if(!cmd->processed(i)) {
      QMessageBox::critical(this,tr("VNC Monitor - Error"),
			    tr("Unknown option")+
			    " \""+cmd->key(i)+"\" given.");
      exit(1);
    }
  }

  setWindowTitle(tr("VNC Monitor")+QString::asprintf(" [v%s]",VERSION));
  setWindowIcon(QPixmap(vncmon_16x16_xpm));
  QFont bold_font(font().family(),font().pointSize(),QFont::Bold);

  d_config=new Config();
  d_config->load();

  //
  // Check (and if needed, create) the VNC config directory
  //
  if(!QDir::home().exists(".vnc")) {
    if(!QDir::home().mkdir(".vnc")) {
      QMessageBox::warning(this,tr("VNC Monitor - Warning"),
			   tr("Unable to create VNC configuration directory!"));
    }
    else {
      if(chmod((QDir::homePath()+"/.vnc").toUtf8(),
	       S_IRUSR|S_IWUSR|S_IXUSR)!=0) {
	QMessageBox::warning(this,tr("VNC Monitor - Warning"),
	     tr("Unable to set permissions on  VNC configuration directory!"));
      }
    }
  }

  d_connection_label=new QLabel(tr("VNC Connections"),this);
  d_connection_label->setFont(bold_font);

  d_connection_model=new ConnectionListModel(this);
  d_connection_model->setFont(font());
  d_connection_model->setWhitelistedAddresses(d_config->whitelist());

  d_connection_listview=new ConnectionListView(this);
  d_connection_listview->setModel(d_connection_model);

  //
  // SIGUSR1 Signal Handler
  //
  struct sigaction action;
  memset(&action,0,sizeof(action));
  action.sa_handler=SigHandler;
  action.sa_flags=SA_RESTART;
  sigaction(SIGUSR1,&action,NULL);
  if(::socketpair(AF_UNIX,SOCK_STREAM,0,global_signal_sockets)!=0) {
    fprintf(stderr,"can't create socket pair: %s\n",strerror(errno));
    exit(1);
  }
  d_signal_notifier=
    new QSocketNotifier(global_signal_sockets[1],QSocketNotifier::Read,this);
  connect(d_signal_notifier,
	  SIGNAL(activated(QSocketDescriptor,QSocketNotifier::Type)),
	  this,SLOT(signalReceivedData()));

  //
  // Startup Timer
  //
  d_startup_timer=new QTimer(this);
  d_startup_timer->setSingleShot(true);
  connect(d_startup_timer,SIGNAL(timeout()),this,SLOT(startupData()));
  d_startup_timer->start(10000);
}


QSize MainWidget::sizeHint() const
{
  return QSize(300,200);
}


void MainWidget::signalReceivedData()
{
  char data[1];
  read(global_signal_sockets[1],data,1);
  setVisible(!isVisible());
}


void MainWidget::startupData()
{
  //
  // Restart the plug-in
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
      bool ok=false;
      QStringList f1=f0.at(i).split(" ",Qt::SkipEmptyParts);
      unsigned pid=f1.at(0).toUInt(&ok);
      if(ok) {
	kill(pid,SIGTERM);
      }
    }
  }
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  e->ignore();
  hide();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  d_connection_label->setGeometry(10,2,width(),20);
  d_connection_listview->setGeometry(10,25,size().width()-20,size().height()-35);
}


int main(int argc,char *argv[]) {
  QApplication a(argc,argv);

  new MainWidget();

  return a.exec();
}
