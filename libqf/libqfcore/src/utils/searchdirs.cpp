//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#include "searchdirs.h"
#include "fileutils.h"
#include "../core/log.h"

#include <QDir>

using namespace qf::core::utils;

//===================================================================
//                 SearchDirs
//===================================================================
void SearchDirs::setDirs(const QString & s)
{
	QStringList sl;
	if(!s.isEmpty()) sl = s.split("::", QString::SkipEmptyParts);
	dirsRef() = sl;
}

void SearchDirs::appendDir(const QString & path)
{
	int ix = dirs().indexOf(path);
	if(ix < 0) dirsRef() << path;
}

void SearchDirs::prependDir(const QString & path)
{
	QStringList &sl = dirsRef();
	if(!sl.isEmpty() && sl[0] == path) return;
	sl.prepend(path);
}

QPair<QString, QString> SearchDirs::findFile2(const QString & _file_name, QVariant *file_info)
{
	Q_UNUSED(file_info);
	qfLogFuncFrame() << _file_name;
	QPair<QString, QString> ret;
	QStringList file_names;
	file_names << _file_name;
	qfDebug() << "findFile2() -" << _file_name;
	foreach(QString file_name, file_names) {
		if(QDir::isAbsolutePath(file_name)) {
			if(QFile::exists(file_name)) {
				ret.second = file_name;
				break;
			}
		}
		else {
			foreach(QString dir, dirs()) {
				QString abs_fn_path = FileUtils::joinPath(dir, file_name);
				qfDebug() << "\t trying:" << abs_fn_path;
				if(QFile::exists(abs_fn_path)) {
					qfDebug() << "\t\t SUCCES";
					//if(dir.endsWith(".rep")) qfInfo() << dir;
					ret.first = dir;
					ret.second = file_name;
					break;
				}
			}
		}
		if(!ret.second.isEmpty()) 
			break;
	}
	ret.first = QDir::fromNativeSeparators(ret.first);
	ret.second = QDir::fromNativeSeparators(ret.second);
	return ret;
}

QString SearchDirs::findFile(const QString& file_name, QVariant *file_info)
{
	Q_UNUSED(file_info);
	QPair<QString, QString> ret2 = findFile2(file_name, file_info);
	QString ret = FileUtils::joinPath(ret2.first, ret2.second);
	return ret;
}

QByteArray SearchDirs::resolveFileNameAndLoadFile(const QString& file_name)
{
	QString fn = findFile(file_name);
	return loadFile(fn);
}

QByteArray SearchDirs::loadFile(const QString& file_name)
{
	QByteArray ret;
	QFile f(file_name);
	if(!f.isOpen()) {
		if(!f.open(QIODevice::ReadOnly)) {
			QString s = QObject::tr("ERROR open file '%1'").arg(f.fileName());
			//if(throw_exc) QF_EXCEPTION(s);
			qfWarning() << s;
		}
	}
	if(f.isOpen()) {
		ret = f.readAll();
	}
	return ret;
}

bool SearchDirs::saveFile(const QString& abs_file_name, const QByteArray& file_content)
{
	bool ret = false;
	QFile f(abs_file_name);
	if(f.open(QFile::WriteOnly)) {
		ret = (f.write(file_content) > 0);
	}
	return ret;
}
