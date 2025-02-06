// connectionlistview.cpp
//
// QListView for network connections.
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

#include <sys/types.h>
#include <signal.h>

#include <QMouseEvent>

#include "connectionlistmodel.h"
#include "connectionlistview.h"

ConnectionListView::ConnectionListView(QWidget *parent)
  : QListView(parent)
{
  d_connection_menu=new QMenu(this);
  connect(d_connection_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));
  d_kick_action=d_connection_menu->addAction(tr("Kick this connection"),this,
					     SLOT(kickConnectionData()));
}


void ConnectionListView::aboutToShowMenuData()
{
}


void ConnectionListView::kickConnectionData()
{
  Connection conn=((ConnectionListModel *)model())->connection(d_mouse_index);
  if(conn.isValid()) {
    kill(conn.pid(),SIGKILL);
  }
}


void ConnectionListView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    d_mouse_index=indexAt(e->pos());
    if(d_mouse_index.isValid()) {
      d_connection_menu->
	popup(QPoint(e->globalPosition().x(),e->globalPosition().y()));
    }
  }
  QWidget::mousePressEvent(e);
}


void ConnectionListView::mouseMoveEvent(QMouseEvent *e)
{
  //    rownum=(e->pos().y()+list_position)/ENDPOINTLIST_ITEM_HEIGHT;
}
