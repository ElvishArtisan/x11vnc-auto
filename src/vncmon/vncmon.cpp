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

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <QApplication>
#include <QMessageBox>
#include <QStringList>

#include "vncmon.h"

MainWidget::MainWidget(QWidget *parent)
  : QWidget(parent)
{
  setWindowTitle(tr("VNC Monitor")+QString::asprintf(" [v%s]",VERSION));

  d_connection_model=new ConnectionListModel(this);
  d_connection_listview=new ConnectionListView(this);
  d_connection_listview->setModel(d_connection_model);

}


QSize MainWidget::sizeHint() const
{
  return QSize(200,100);
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
