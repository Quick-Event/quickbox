#include "filedialog.h"

#include <qf/core/log.h>
#include <qf/core/utils/fileutils.h>

#include <QApplication>
#include <QSettings>

using namespace qf::qmlwidgets::dialogs;

FileDialog::FileDialog(QWidget *parent, Qt::WindowFlags flags) :
	QFileDialog(parent, flags)
{
}

QString FileDialog::getOpenFileName(QWidget *parent, const QString &caption,
									const QString &_dir, const QString &filter,
									QString *selectedFilter, QFileDialog::Options options)
{
	QString dir = _dir;
	if(dir.isEmpty()) {
		dir = recentOpenFileDir();
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QString ret = QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
	if(!ret.isEmpty()) {
		setRecentOpenFileDir(qf::core::utils::FileUtils::path(ret));
	}
	return ret;
}

QStringList FileDialog::getOpenFileNames(QWidget *parent, const QString &caption,
										 const QString &_dir, const QString &filter,
										 QString *selectedFilter, QFileDialog::Options options)
{
	QString fn = _dir;
	QString dir = qf::core::utils::FileUtils::path(fn);
	if(dir.isEmpty()) {
		dir = recentOpenFileDir();
		fn = qf::core::utils::FileUtils::joinPath(dir, fn);
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QStringList ret = QFileDialog::getOpenFileNames(parent, caption, fn, filter, selectedFilter, options);
	if(!ret.isEmpty()) {
		setRecentOpenFileDir(qf::core::utils::FileUtils::path(ret[0]));
	}
	return ret;
}

QString FileDialog::getSaveFileName(QWidget * parent, const QString & caption,
									const QString & _dir, const QString & filter,
									QString * selectedFilter, QFileDialog::Options options)
{
	qfLogFuncFrame();
	QString fn = _dir;
	qDebug() << "\t fn:" << fn;
	QString dir = qf::core::utils::FileUtils::path(fn);
	qDebug() << "\t dir:" << dir << "isAbsolutePath:" << QDir::isAbsolutePath(dir);
	if(dir.isEmpty() || !QDir::isAbsolutePath(dir)) {
		dir = recentSaveFileDir();
		fn = qf::core::utils::FileUtils::joinPath(dir, fn);
	}
	//options |= QFileDialog::DontUseNativeDialog;
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	qfDebug() << "\t fn2:" << fn << "parent:" << parent;
	QString ret = QFileDialog::getSaveFileName(parent, caption, fn, filter, selectedFilter, options);
	qfDebug() << "\t ret:" << ret;
	if(!ret.isEmpty()) {
		setRecentSaveFileDir(qf::core::utils::FileUtils::path(ret));
	}
	return ret;
}

QString FileDialog::getExistingDirectory(QWidget *parent, const QString &caption, const QString &_dir, QFileDialog::Options options)
{
	QString dir = _dir;
	if(dir.isEmpty()) {
		dir = recentOpenFileDir();
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QString ret = QFileDialog::getExistingDirectory(parent, caption, dir, options);
	if(!ret.isEmpty()) {
		setRecentOpenFileDir(ret);
	}
	return ret;
}

static const auto RECENT_OPEN_FILE_DIR_KEY = QStringLiteral("app/FileDialog/recentOpenFileDir");
static const auto RECENT_SAVE_FILE_DIR_KEY = QStringLiteral("app/FileDialog/recentSaveFileDir");

QString FileDialog::recentOpenFileDir()
{
	QSettings settings;
	return settings.value(RECENT_OPEN_FILE_DIR_KEY).toString();
}

void FileDialog::setRecentOpenFileDir(const QString &dir)
{
	QSettings settings;
	settings.setValue(RECENT_OPEN_FILE_DIR_KEY, dir);
}

QString FileDialog::recentSaveFileDir()
{
	QSettings settings;
	return settings.value(RECENT_SAVE_FILE_DIR_KEY).toString();
}

void FileDialog::setRecentSaveFileDir(const QString &dir)
{
	QSettings settings;
	settings.setValue(RECENT_SAVE_FILE_DIR_KEY, dir);
}
