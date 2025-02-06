// connectionlistmodel.h
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

#ifndef CONNECTIONLISTMODEL_H
#define CONNECTIONLISTMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QHostAddress>
#include <QHostInfo>
#include <QProcess>
#include <QStringList>
#include <QTimer>

#define CONNECTIONLISTMODEL_SCAN_INTERVAL 5000

class Connection
{
 public:
  Connection(int pid,const QHostAddress &addr,int port);
  Connection();
  bool isValid() const;
  int pid() const;
  QHostAddress address() const;
  int port() const;
  QString hostname() const;
  void setHostname(const QString &str);
  QString idString() const;
  Connection operator=(const Connection &rhs);
  bool operator==(const Connection &other) const;
  
 private:
  int d_pid;
  QHostAddress d_address;
  int d_port;
  QString d_hostname;
};




class ConnectionListModel : public QAbstractListModel
{
  Q_OBJECT
 public:
  ConnectionListModel(QWidget *parent=0);
  Connection connection(const QModelIndex &index) const;
  QList<QHostAddress> whitelistedAddresses() const;
  void setWhitelistedAddresses(const QList<QHostAddress> &addrs);
  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;

 private slots:
  void scanData();
  void hostLookupFinished(const QHostInfo &hinfo);

 private:
  void UpdateConnections();
  void LoadConnectionTable(QMap<int,Connection> &conntab);
  QTimer *d_scan_timer;
  QProcess *d_scan_process;
  QDir *d_proc_dir;
  QList<Connection> d_connections;
  QList<QHostAddress> d_whitelisted_addresses;
};


#endif  // CONNECTIONLISTMODEL_H
