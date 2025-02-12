// config.h
//
// Configuration for the vncmon(1) plug-in.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of version 2.1 of the GNU Lesser General Public
//    License as published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, 
//    Boston, MA  02111-1307  USA
//

#ifndef CONFIG_H
#define CONFIG_H

#include <QHostAddress>
#include <QList>
#include <QPoint>

#include "profile.h"

#define VNCMON_CONFIG_FILE "/etc/vncmon.conf"

class Config
{
 public:
  Config();
  QList<QHostAddress> whitelist() const;
  void load();
  void clear();

 private:
  QList<QHostAddress> d_whitelist;
};


#endif  // CONFIG_H
