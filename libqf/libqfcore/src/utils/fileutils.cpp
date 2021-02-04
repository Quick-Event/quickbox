#include "fileutils.h"

#include "../core/log.h"
#include "../core/string.h"

#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QUrl>

using namespace qf::core::utils;

QString FileUtils::appNameLower()
{
	QString s = QCoreApplication::applicationName();
	if(s.isEmpty()) {
		s = QFileInfo(QCoreApplication::applicationFilePath()).baseName();
		int ix = s.indexOf('.');
		if(ix > 0)
			s = s.mid(0, ix);
		s = s.toLower();
	}
	return s;
}

QString FileUtils::unixSeparators(const QString &file_name)
{
	QChar sep = QDir::separator();
	QString fn = file_name;
	fn = fn.replace(sep, '/');
	return fn;
}

QString FileUtils::path(const QString &file_name)
{
	QString fn = FileUtils::unixSeparators(file_name);
	int ix = fn.lastIndexOf('/');
	if(ix < 0) return QString();
	return fn.mid(0, ix+1);
}

QString FileUtils::file(const QString &file_name)
{
	QString fn = FileUtils::unixSeparators(file_name);
	int ix = fn.lastIndexOf('/');
	if(ix < 0) return fn;
	return fn.mid(ix + 1);
}

QString FileUtils::extension(const QString &file_name)
{
	QString fn = file_name;
	int ix = fn.lastIndexOf('.');
	if(ix < 0)
		return QString();
	return fn.mid(ix + 1).trimmed();
}

QString FileUtils::stripExtension(const QString &file_name)
{
	QString fn = file_name;
	int ix = fn.lastIndexOf('.');
	if(ix < 0)
		return fn;
	return fn.mid(0, ix);
}

QString FileUtils::stripExtensions(const QString &file_name)
{
	QString fn = file_name;
	QString p = FileUtils::path(fn);
	fn = FileUtils::baseName(fn);
	if(!p.isEmpty())
		fn = FileUtils::joinPath(p, fn);
	return fn;
}

QString FileUtils::baseName(const QString &path)
{
	QString s = file(path);
	int ix = s.indexOf('.');
	if(ix < 0)
		return s;
	return s.mid(0, ix);
}

QString FileUtils::homeDir()
{
	QString s = QDir::homePath();
	qfDebug() << QF_FUNC_NAME << s;
	return s;
}

QString FileUtils::currDir()
{
	return QDir::currentPath();
}

QString FileUtils::osTempDir()
{
	return QDir::tempPath();
}

QString FileUtils::appDir()
{
	return QCoreApplication::applicationDirPath();
}

QString FileUtils::createUserTempDir(const QString &user_name)
{
	QString app_name =  appNameLower();
	QString os_temp_dir =  FileUtils::osTempDir();
	QString tmp_dir = FileUtils::joinPath(os_temp_dir, app_name);
	tmp_dir = FileUtils::joinPath(tmp_dir, user_name);
	tmp_dir = FileUtils::joinPath(tmp_dir, "tmp");
	QDir d(tmp_dir);
	if(!d.exists()) {
		if(!d.mkpath(tmp_dir)) {
			qfError() << QString("Cann't create temporary directory '%1'").arg(tmp_dir);
			return QString();
		}
	}
	return tmp_dir;
}

QString FileUtils::joinPath(const QString &path1, const QString &path2)
{
	qfLogFuncFrame() << path1 << path2;
	qf::core::String ret = path1;
	if(ret.isEmpty()) {
		ret = path2;
	}
	else {
		if(!path2.isEmpty() && path2[0] != '/' && ret.value(-1) != '/')
			ret += "/";
		ret += path2;
	}
	qfDebug() << "\t before cleanPath:" << ret;
	/// QDir::cleanPath(ret); cannot clean whole path in one iteration, a/b/../../c is changed to a/../c
	ret = FileUtils::cleanPath(ret);
	qfDebug() << "\t return:" << ret;
	return ret;
}

QString FileUtils::joinPath(const QStringList & paths)
{
	QString s = paths.join("/");
	s = QDir::cleanPath(s);
	return s;
}

QString FileUtils::cleanPath(const QString& path)
{
	/// pozor cleanPath: knihazakazek/..//companyheadercontent.inc.xml vyrobi /companyheadercontent.inc.xml
	/// ^^^ nutno overit

	/// QDir::cleanPath(fs); nedokaze odstranit v jenom pruchodu neco jako a/b/../../c, vyrobi a/../c
	QString ret = QDir::cleanPath(path);
	while(ret.contains("/../")) ret = QDir::cleanPath(ret);
	return ret;
}

bool FileUtils::ensurePath(const QString & _path)
{
	QDir dir(_path);
	qf::core::String path = QDir::fromNativeSeparators(dir.absolutePath());
	QStringList sl = path.splitAndTrim('/', '"', true, QString::SkipEmptyParts);
	if(path.value(1) == ':') {
		/// windows
		dir = QDir(sl[0]);
		sl = sl.mid(1);
	}
	else {
		/// unix
		dir = QDir("/");
		if(sl.count() > 0 && sl[0].isEmpty())
			sl = sl.mid(1);
	}
	return dir.mkpath(sl.join("/"));
}

QUrl FileUtils::saveText(const QString & text, const QString & filename_with_path, const char *codec_name)
{
	QUrl url;
	QString dir = FileUtils::path(filename_with_path);
	QString file = FileUtils::file(filename_with_path);
	if(ensurePath(dir)) {
		QFile f(filename_with_path);
		if(f.open(QIODevice::WriteOnly)) {
			QTextStream ts(&f);
			ts.setCodec(codec_name);
			ts << text;
			f.close();
			QDir d(dir);
			QString abs_file_name = d.absoluteFilePath(file);
			url = QUrl::fromLocalFile(abs_file_name);
			//url = QUrl(f.fileName());
			//url.setScheme("file");
		}
	}
	return url;
}
#if 0
bool FileUtils::removeDirContent(const QDir & dir)
{
	/// zda se, ze ve windows dokaze tahle funkce vygenerovat 99% CPU, asi kdyz neco nejde vymazat, nevim ...
	qfLogFuncFrame() << dir.absolutePath();
	bool ret = true;
	QDir d = dir;
	d.setNameFilters(QStringList());
	foreach(QFileInfo fi, d.entryInfoList()) {
		if(fi.isFile()) {
			QString fn = fi.absoluteFilePath();
			qfDebug() << "\t removing ordinary file:" << fn;
			if(!QFile::remove(fn)) {ret = false; break;}
		}
		else if(fi.isDir()) {
			QString dir_name = fi.absoluteFilePath();
			if(d.absolutePath().startsWith(dir_name)) continue; /// current dir nebo parentdir
			if(!fi.isSymLink()) {
				qfDebug() << "\t removing content of directory:" << dir_name;
				if(!removeDirContent(QDir(dir_name))) {ret = false; break;}
			}
			qfDebug() << "\t removing directory:" << QDir(dir_name).dirName();
			if(!d.rmdir(QDir(dir_name).dirName())) {ret = false; break;}
		}
	}
	qfDebug() << "\t return:" << ret;
	return ret;
}
#endif
bool FileUtils::removeDir(const QString &_dir_name)
{
	bool ret = true;
	QDir dir(_dir_name);

	if (dir.exists()) {
		QString dir_name = dir.absolutePath();
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				if(!info.isSymLink()) {
					ret = removeDir(info.absoluteFilePath());
				}
			}
			else {
				ret = QFile::remove(info.absoluteFilePath());
			}
			if(!ret) {
				qfWarning() << QString("ERROR removing path: '%1'").arg(info.absoluteFilePath());
				break;
			}
		}
		ret = dir.rmdir(dir_name);
	}
	return ret;
}
