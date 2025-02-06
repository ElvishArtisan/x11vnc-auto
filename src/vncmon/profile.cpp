// profile.cpp
//
// A container class for profile lines.
//
// (C) Copyright 2013-2024 Fred Gleason <fredg@paravelsystems.com>
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
// EXEMPLAR_VERSION: 2.0.1
//

#include <stdio.h>

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextStream>

#include "profile.h"

#define __PROFILE_SECTION_ID_DELIMITER "|"
#define __PROFILE_DEFAULT_SECTION_ID "Default"

Profile::Profile(bool use_section_ids)
{
  d_use_section_ids=use_section_ids;
}


QStringList Profile::sectionNames() const
{
  return d_blocks.keys();
}


QString Profile::source() const
{
  return profile_source;
}


bool Profile::addSource(const QStringList &values)
{
  QString block_name;
  QMap<QString,QStringList> block_lines;

  for(int i=0;i<values.size();i++) {
    QString line=values.at(i);
    if((line.left(1)=="[")&&(line.right(1)=="]")) {  // Block Starts
      if(!block_name.isEmpty()) {
	ProcessBlock(block_name,block_lines);
      }
      block_name=line.mid(1,line.size()-2);
      block_lines.clear();
    }
    else {
      if((!line.isEmpty())&&(line.left(1)!=";")&&(line.left(1)!="#")) {
	QStringList f0=line.split("=",Qt::KeepEmptyParts);
	QString tag=f0.at(0);
	f0.removeFirst();
	QStringList f1=block_lines.value(tag,QStringList());
	f1.push_back(f0.join("="));
	block_lines[tag]=f1;
      }
    }
  }
  if(!block_name.isEmpty()) {
    ProcessBlock(block_name,block_lines);
  }
  
  return true;
}


bool Profile::loadFile(const QString &filename,QString *err_msg)
{
  QFile data(filename);
  if(!data.open(QFile::ReadOnly)) {
    *err_msg="unable to open file";
    return false;
  }
  QTextStream in(&data);
  QString line;
  QStringList values;
  while(in.readLineInto(&line)) {
    values.push_back(line.trimmed());
  }
  addSource(values);
  if(err_msg!=NULL) {
    *err_msg=
      QString::asprintf("loaded file \"%s\"",filename.toUtf8().constData());
  }

  return true;
}


int Profile::loadDirectory(const QString &dirpath,const QString &glob_template,
			   QStringList *err_msgs)
{
  QString err_msg;
  QDir dir(dirpath);
  int ret=0;

  if(!dir.exists()) {
    if(err_msgs!=NULL) {
      err_msgs->push_back(QObject::tr("no such directory"));
      return -1;
    }
  }
  if(!dir.isReadable()) {
    if(err_msgs!=NULL) {
      err_msgs->push_back(QObject::tr("directory is not readable"));
      return -1;
    }
  }
  QStringList name_filters;
  name_filters.push_back(glob_template);
  QStringList filenames=dir.entryList(name_filters,QDir::Files,QDir::Name);
  for(int i=0;i<filenames.size();i++) {
    if(loadFile(dir.path()+"/"+filenames.at(i),&err_msg)) {
      if(err_msgs!=NULL) {
	err_msgs->push_back(QString::asprintf("loaded file \"%s/%s\"",
				     dir.path().toUtf8().constData(),
				     filenames.at(i).toUtf8().constData()));
      }
      ret++;
    }
    else {
      if(err_msgs!=NULL) {
	err_msgs->
	  push_back(QString::asprintf("failed to load file \"%s\": %s",
				      filenames.at(i).toUtf8().constData(),
				      err_msg.toUtf8().constData()));
      }
    }
  }

  return ret;
}


int Profile::load(const QString &glob_path,QStringList *err_msgs)
{
  QString err_msg;
  QFileInfo finfo(glob_path);
  if(finfo.isFile()) {
    int ret=loadFile(glob_path,&err_msg);
    if(err_msgs!=NULL) {
      err_msgs->push_back(err_msg);
    }
    return ret;
  }

  QStringList f0=glob_path.split("/",Qt::KeepEmptyParts);
  QString glob_template=f0.last();
  f0.removeLast();
  QString dir_path=f0.join("/");
  
  return loadDirectory(dir_path,glob_template,err_msgs);
}


QStringList Profile::sections() const
{
  QStringList ret;

  for(QMap<QString,QMap<QString,QStringList> >::const_iterator it=
	d_blocks.begin();it!=d_blocks.end();it++) {
    QStringList f0=it.key().split(__PROFILE_SECTION_ID_DELIMITER);
    if(!ret.contains(f0.first())) {
      ret.push_front(f0.first());
    }
  }

  return ret;
}


QStringList Profile::sectionIds(const QString &section) const
{
  QStringList ret;

  for(QMap<QString,QMap<QString,QStringList> >::const_iterator it=
	d_blocks.begin();it!=d_blocks.end();it++) {
    QStringList f0=it.key().split(__PROFILE_SECTION_ID_DELIMITER);
    if((f0.size()==2)&&(f0.first()==section)) {
      ret.push_back(f0.last());
    }
  }

  return ret;
}


QString Profile::stringValue(const QString &section,const QString &tag,
			     const QString &default_str,bool *found)
{
  QStringList values=stringValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_str;
  }
  return values.first();
}


QStringList Profile::stringValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QStringList();
  }
  return block.value(tag);
}


QStringList Profile::stringValues(const QString &section,
				  const QString &section_id,
				  const QString &tag) const
{
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return QStringList();
  }
  
  return block.value(tag);
}


int Profile::intValue(const QString &section,const QString &tag,
		      int default_value,bool *found)
{
  QList<int> values=intValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_value;
  }
  return values.first();
}


QList<int> Profile::intValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<int>();
  }
  QList<int> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toInt());
  }
  return ret;
}


QList<int> Profile::intValues(const QString &section,const QString &section_id,
			      const QString &tag)
{
  QList<int> ret;
  
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toInt());
  }
  
  return ret;
}


int Profile::hexValue(const QString &section,const QString &tag,
		       int default_value,bool *found)
{
  QList<int> values=hexValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_value;
  }
  return values.first();
}


QList<int> Profile::hexValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<int>();
  }
  QList<int> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toInt(NULL,16));
  }
  return ret;
}


QList<int> Profile::hexValues(const QString &section,const QString &section_id,
			      const QString &tag)
{
  QList<int> ret;
  
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toInt(NULL,16));
  }
  
  return ret;
}


double Profile::doubleValue(const QString &section,const QString &tag,
			    double default_value,bool *found)
{
  QList<double> values=doubleValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_value;
  }
  return values.first();
}


QList<double> Profile::doubleValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<double>();
  }
  QList<double> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toDouble());
  }
  return ret;
}


QList<double> Profile::doubleValues(const QString &section,
				    const QString &section_id,
				    const QString &tag)
{
  QList<double> ret;
  
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(values.at(i).toDouble());
  }
  
  return ret;
}


bool Profile::boolValue(const QString &section,const QString &tag,
			 bool default_value,bool *found)
{
  QList<bool> values=boolValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_value;
  }
  return values.first();
}


QList<bool> Profile::boolValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<bool>();
  }
  QList<bool> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back((values.at(i).toLower()=="yes")||
		  (values.at(i).toLower()=="true")||
		  (values.at(i).toLower()=="on")||
		  (values.at(i).toLower()=="1"));
  }
  return ret;
}


QList<bool> Profile::boolValues(const QString &section,
				const QString &section_id,
				const QString &tag)
{
  QList<bool> ret;
  
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back((values.at(i).toLower()=="yes")||
		  (values.at(i).toLower()=="true")||
		  (values.at(i).toLower()=="on")||
		  (values.at(i).toLower()=="1"));
  }
  
  return ret;
}


QTime Profile::timeValue(const QString &section,const QString &tag,
			   const QTime &default_value,bool *found)
{
  QList<QTime> values=timeValues(section,tag);
  if(found!=NULL) {
    *found=values.size()>0;
  }
  if(values.size()==0) {
    return default_value;
  }
  return values.first();
}


QList<QTime> Profile::timeValues(const QString &section,const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<QTime>();
  }
  QList<QTime> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    QStringList fields=values.at(i).split(":");
    if(fields.size()==2) {
      ret.push_back(QTime(fields.at(0).toInt(),fields.at(1).toInt(),0));
    }
    else {
      if(fields.size()==3) {
	ret.push_back(QTime(fields.at(0).toInt(),fields.at(1).toInt(),
			    fields.at(2).toInt()));
      }
      else {
	ret.push_back(QTime());
      }
    }
  }
  return ret;
}


QList<QTime> Profile::timeValues(const QString &section,
				 const QString &section_id,
				 const QString &tag)
{
  QList<QTime> ret;

  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    QStringList fields=values.at(i).split(":");
    if(fields.size()==2) {
      ret.push_back(QTime(fields.at(0).toInt(),fields.at(1).toInt(),0));
    }
    else {
      if(fields.size()==3) {
	ret.push_back(QTime(fields.at(0).toInt(),fields.at(1).toInt(),
			    fields.at(2).toInt()));
      }
      else {
	ret.push_back(QTime());
      }
    }
  }
  
  return ret;
}


QHostAddress Profile::addressValue(const QString &section,const QString &tag,
				  const QHostAddress &default_value,bool *found)
{
  return QHostAddress(stringValue(section,tag,default_value.toString(),found));
}


QHostAddress Profile::addressValue(const QString &section,const QString &tag,
				     const QString &default_value,bool *found)
{
  return addressValue(section,tag,QHostAddress(default_value),found);
}


QList<QHostAddress> Profile::addressValues(const QString &section,
					   const QString &tag)
{
  QMap<QString,QStringList> block=d_blocks.value(section);
  if(block.size()==0) {
    return QList<QHostAddress>();
  }
  QList<QHostAddress> ret;
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(QHostAddress(values.at(i)));
  }
  return ret;
}


QList<QHostAddress> Profile::addressValues(const QString &section,
					   const QString &section_id,
					   const QString &tag)
{
  QList<QHostAddress> ret;
  
  QMap<QString,QStringList> block=
    d_blocks.value(section+__PROFILE_SECTION_ID_DELIMITER+section_id);
  if(block.size()==0) {
    return ret;
  }
  QStringList values=block.value(tag);
  for(int i=0;i<values.size();i++) {
    ret.push_back(QHostAddress(values.at(i)));
  }
  
  return ret;
}


void Profile::clear()
{
  profile_source="";
  d_blocks.clear();
}


QString Profile::dump() const
{
  QString ret;
  
  for(QMap<QString,QMap<QString,QStringList> >::const_iterator it0=
	d_blocks.begin();it0!=d_blocks.end();it0++) {
    QStringList section=it0.key().split("|");
    ret+=QString::asprintf("[%s]\n",section.first().toUtf8().constData());
    if(d_use_section_ids) {
      ret+=QString::asprintf("Id=%s\n",section.last().toUtf8().constData());
    }
    for(QMap<QString,QStringList>::const_iterator it1=it0.value().begin();
	it1!=it0.value().end();it1++) {
      for(int i=0;i<it1.value().size();i++) {
	if((!d_use_section_ids)||(it1.key()!="Id")) {
	  ret+=QString::asprintf("%s=%s\n",it1.key().toUtf8().constData(),
				 it1.value().at(i).toUtf8().constData());
	}
      }
    }
  }

  return ret;
}


void Profile::ProcessBlock(const QString &name,
			   const QMap<QString,QStringList> &lines)
{
  QString block_name=name;
  if(d_use_section_ids) {
    QStringList ids=lines.value("Id");
    QString id=__PROFILE_DEFAULT_SECTION_ID;
    if(ids.size()>0) {
      id=ids.first();
    }
    block_name=name+__PROFILE_SECTION_ID_DELIMITER+id;
  }

  if(!lines.isEmpty()) {
    QMap<QString,QStringList> block=
      d_blocks.value(block_name,QMap<QString,QStringList>());

    for(QMap<QString,QStringList>::const_iterator it=lines.begin();
	it!=lines.end();it++) {
      block[it.key()].append(it.value());
    }
    if(d_use_section_ids) {
      d_blocks[block_name]=block;
    }
    else {
      d_blocks[name]=block;
    }
  }
}


QStringList Profile::InvertList(const QStringList &list) const
{
  QStringList ret;

  for(int i=0;i<list.size();i++) {
    ret.push_back(list.at(list.size()-i-1));
  }
  
  return ret;
}


void Profile::DumpList(const QString &title,const QStringList &list) const
{
  printf("%s\n",title.toUtf8().constData());
  for(int i=0;i<list.size();i++) {
    printf("[%d]: %s\n",i,list.at(i).toUtf8().constData());
  }
}
