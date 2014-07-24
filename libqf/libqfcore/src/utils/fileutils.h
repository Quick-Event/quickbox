#ifndef QF_CORE_UTILS_FILEUTILS_H
#define QF_CORE_UTILS_FILEUTILS_H

#include "../core/coreglobal.h"

#include <QString>

class QDir;
class QUrl;

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT FileUtils
{
private:
	static QString appNameLower();
public:
	//! Convert host separator to '/'.
	static QString unixSeparators(const QString &file_name);
	//! Return path of \a file_name . Path (if any) allways contains trailing '/'.
	static QString path(const QString &file_name);
	static QString dir(const QString &file_name) {return path(file_name);}
	//! Return file name with extension. Remove path if any.
	static QString file(const QString &file_name);
	//! Return extension of \a file_name .
	static QString extension(const QString &file_name);
	//! Strip the last extension of \a file_name . Function does not strip the path.
	static QString stripExtension(const QString &file_name);
	//! Strip all extensions of \a file_name . Function does not strip the path.
	static QString stripExtensions(const QString &file_name);
	//! Strip path and everything after the first dot in file name;
	static QString baseName(const QString &path);

	/// vraci $HOME aktualniho uzivatele, zkousi vsechno, co se da,
	/// aby na to prisel.
	static QString homeDir();
	/// vytvori neco jako $TMP/app_name/user_name/tmp a vrati to
	static QString createUserTempDir(const QString &user_name);
	//! Returns OS temp directory;
	static QString osTempDir();
	//! pwd.
	static QString currDir();
	//! dir where application file resist.
	static QString appDir();
	/// spoji dve cesty a vynda/doplni unixove oddelovace '/'
	static QString joinPath(const QString &path1, const QString &path2);
	/// spoji cesty a vynda/doplni unixove oddelovace '/'
	static QString joinPath(const QStringList &paths);

	static QString cleanPath(const QString &path);

	//! create path \a path if it does not exist allready.
	static bool ensurePath(const QString &path);

	//! vymaze kompletne obsah adresare vcetne podadresaru
	/// zda se, ze ve windows dokaze tahle funkce vygenerovat 99% CPU, asi kdyz neco nejde vymazat, nevim ...
	//static bool removeDirContent(const QDir &dir);
	/// tadyta verze zda se nezlobi
	static bool removeDir(const QString &dir_name);

	static QUrl saveText(const QString &text, const QString &filename_with_path, const char *codec_name = "UTF-8");
};

}}}

#endif
