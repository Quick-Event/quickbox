//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_UTILS_SEARCHDIRS_H
#define QF_CORE_UTILS_SEARCHDIRS_H

#include "../core/coreglobal.h"

#include <QStringList>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT SearchDirs
{
public:
	SearchDirs() { }
	//virtual ~QFSearchDir() {}
public:
	QStringList dirs() const {return const_cast<SearchDirs*>(this)->dirsRef();}
	QStringList& dirsRef() {return f_dirList;}
	void setDirs(const QStringList &sl) {dirsRef() = sl;}
	/// oddelovac je kvuli windows "::"
	void setDirs(const QString &s);
	//! Pokud uz tam \a path je, neprida se, stejne by na ni nedosla rada a umoznuje to volat funkci opakovane bez nasledku.
	void appendDir(const QString &path);
	//! Pokud uz tam \a path je na prvnim miste, neprida se, bylo by to zbytecny a umoznuje to volat funkci opakovane bez nasledku.
	void prependDir(const QString &path);
	QString findFile(const QString &file_name, QVariant *file_info = NULL);
	/// if file_name is absolute path, then check if it exists
	/// if file_name is relative path, scan searchDirs() and return first file that exists or empty string.
	/// @return (to_abs_path_prefix, filename)
	virtual QPair<QString, QString> findFile2(const QString &file_name, QVariant *file_info = NULL);

	QByteArray resolveFileNameAndLoadFile(const QString &file_name);
	/// uz neresi lokalizaci
	virtual QByteArray loadFile(const QString &abs_file_name);
	/// uz neresi lokalizaci
	virtual bool saveFile(const QString &abs_file_name, const QByteArray &file_content);
protected:
	QStringList f_dirList;
};

}}}

#endif // QF_CORE_UTILS_SEARCHDIRS_H

