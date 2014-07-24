#include "filedialog.h"

#include <qf/core/log.h>
#include <qf/core/utils/fileutils.h>

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
	qDebug() << "\t fn2:" << fn;
	QString ret = QFileDialog::getSaveFileName(parent, caption, fn, filter, selectedFilter, options);
	if(!ret.isEmpty()) {
		s_recentSaveFileDir = qf::core::utils::FileUtils::path(ret);
	}
	return ret;
}
