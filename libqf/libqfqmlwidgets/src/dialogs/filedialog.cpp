#include "filedialog.h"

#include <qf/core/log.h>
#include <qf/core/utils/fileutils.h>

#include <QApplication>

using namespace qf::qmlwidgets::dialogs;

QString FileDialog::s_recentOpenFileDir;
QString FileDialog::s_recentSaveFileDir;

FileDialog::FileDialog(QWidget *parent, Qt::WindowFlags flags) :
	QFileDialog(parent, flags)
{
}

QString FileDialog::getOpenFileName(QWidget *parent, const QString &caption,
									const QString &_dir, const QString &filter,
									QString *selectedFilter, QFileDialog::Options options)
{
	QString fn = _dir;
	QString dir = qf::core::utils::FileUtils::path(fn);
	if(dir.isEmpty()) {
		dir = s_recentOpenFileDir;
		fn = qf::core::utils::FileUtils::joinPath(dir, fn);
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QString ret = QFileDialog::getOpenFileName(parent, caption, fn, filter, selectedFilter, options);
	if(!ret.isEmpty()) {
		s_recentOpenFileDir = qf::core::utils::FileUtils::path(ret);
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
		dir = s_recentOpenFileDir;
		fn = qf::core::utils::FileUtils::joinPath(dir, fn);
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QStringList ret = QFileDialog::getOpenFileNames(parent, caption, fn, filter, selectedFilter, options);
	if(!ret.isEmpty()) {
		s_recentOpenFileDir = qf::core::utils::FileUtils::path(ret[0]);
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
		dir = s_recentSaveFileDir;
		fn = qf::core::utils::FileUtils::joinPath(dir, fn);
	}
	//options |= QFileDialog::DontUseNativeDialog;
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	qfDebug() << "\t fn2:" << fn << "parent:" << parent;
	QString ret = QFileDialog::getSaveFileName(parent, caption, fn, filter, selectedFilter, options);
	qfDebug() << "\t ret:" << ret;
	if(!ret.isEmpty()) {
		s_recentSaveFileDir = qf::core::utils::FileUtils::path(ret);
	}
	return ret;
}

QString FileDialog::getExistingDirectory(QWidget *parent, const QString &caption, const QString &_dir, QFileDialog::Options options)
{
	QString dir = _dir;
	if(dir.isEmpty()) {
		dir = s_recentOpenFileDir;
	}
	if(parent == nullptr)
		parent = QApplication::activeWindow();
	QString ret = QFileDialog::getExistingDirectory(parent, caption, dir, options);
	if(!ret.isEmpty()) {
		s_recentOpenFileDir = ret;
	}
	return ret;
}
