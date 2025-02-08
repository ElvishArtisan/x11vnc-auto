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
#include <sys/types.h>
#include <unistd.h>

#include <QApplication>
#include <QMessageBox>
#include <QStringList>

#include "cmdswitch.h"
#include "vncmon.h"

static int global_signal_sockets[2];

void SigHandler(int signum)
{
  char data[1]={0};

  write(global_signal_sockets[0],data,1);
}


MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent,Qt::FramelessWindowHint)
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

  d_profile=new Profile();
  if(!d_profile->loadFile(VNCMON_CONFIG_FILE,&err_msg)) {
    QMessageBox::critical(this,tr("VNC Monitor - Error"),err_msg);
    exit(1);
  }

  d_connection_model=new ConnectionListModel(this);
  d_connection_model->setFont(font());
  QList<QHostAddress> addrs=d_profile->addressValues("Vncmon","Whitelist");
  d_connection_model->setWhitelistedAddresses(addrs);
    
  d_connection_listview=new ConnectionListView(this);
  d_connection_listview->setModel(d_connection_model);

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
}


QSize MainWidget::sizeHint() const
{
  return QSize(200,100);
}


void MainWidget::signalReceivedData()
{
  char data[1];
  read(global_signal_sockets[1],data,1);

  if(isVisible()) {
    hide();
  }
  else {
    int width=0;
    int height=0;
    for(int i=0;i<d_connection_model->rowCount(QModelIndex());i++) {
      QSize s=d_connection_model->data(d_connection_model->index(i,0),
				       Qt::SizeHintRole).toSize();
      height+=s.height();
      if(s.width()>width) {
	width=s.width();
      }
    }
    show();
    printf("size: %dx%d\n",width,height);
    resize(50+width,24+height);
  }
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  d_connection_listview->setGeometry(0,0,size().width(),size().height());
}


int main(int argc,char *argv[]) {
  QApplication a(argc,argv);

  MainWidget *w=new MainWidget();
  w->show();

  return a.exec();
}
