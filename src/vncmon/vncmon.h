// vncmon.h
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

#ifndef VNCMON_H
#define VNCMON_H

#include <QWidget>

#include "connectionlistmodel.h"
#include "connectionlistview.h"
#include "profile.h"

#define VNCMON_SCAN_INTERVAL 5000
#define VNCMON_CONFIG_FILE "/etc/vncmon.conf"
#define VNCMON_USAGE "[options]\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  ConnectionListView *d_connection_listview;
  ConnectionListModel *d_connection_model;
  Profile *d_profile;
};


#endif  // VNCMON_H
