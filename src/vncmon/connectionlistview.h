// connectionlistview.h
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

#ifndef CONNECTIONLISTVIEW_H
#define CONNECTIONLISTVIEW_H

#include <QListView>
#include <QMenu>

class ConnectionListView : public QListView
{
  Q_OBJECT
 public:
  ConnectionListView(QWidget *parent=0);

 private slots:
  void aboutToShowMenuData();
  void kickConnectionData();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

 private:
  QMenu *d_connection_menu;
  QAction *d_kick_action;
  QPointF d_mouse_position;
  QModelIndex d_mouse_index;
};


#endif  // CONNECTIONLISTVIEW_H
