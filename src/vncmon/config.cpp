// config.cpp
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

#include <QDir>

#include "config.h"

Config::Config()
{
  clear();
}


QList<QHostAddress> Config::whitelist() const
{
  return d_whitelist;
}


void Config::load()
{
  QString err_msg;
  
  clear();
  Profile *p=new Profile();
  p->loadFile(VNCMON_CONFIG_FILE,&err_msg);
  p->loadFile(QDir::homePath()+"/.vnc/vncmonrc",&err_msg);
  d_whitelist=p->addressValues("Vncmon","Whitelist");
  delete p;
}


void Config::clear()
{
  d_whitelist.clear();
}
