#ifndef QF_QMLWIDGETS_DIALOGS_FILEDIALOG_H
#define QF_QMLWIDGETS_DIALOGS_FILEDIALOG_H

#include "../qmlwidgetsglobal.h"

#include <QFileDialog>

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT FileDialog : public QFileDialog
{
	Q_OBJECT
public:
	explicit FileDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
public:
	static QString getOpenFileName(QWidget *parent = nullptr, const QString &caption = QString(),
				const QString &dir = QString(), const QString &filter = QString(),
	            QString *selectedFilter = 0, QFileDialog::Options options = QFileDialog::Options());
	static QStringList getOpenFileNames(QWidget *parent = nullptr, const QString &caption = QString(),
				const QString &dir = QString(), const QString &filter = QString(),
	            QString *selectedFilter = 0, QFileDialog::Options options = QFileDialog::Options());
	static QString getSaveFileName(QWidget * parent = 0, const QString & caption = QString(),
				const QString & dir = QString(), const QString & filter = QString(),
	            QString * selectedFilter = 0, QFileDialog::Options options = QFileDialog::Options());
	static QString getExistingDirectory(QWidget * parent = 0, const QString & caption = QString(),
				const QString & dir = QString(), Options options = ShowDirsOnly);
private:
	static QString recentOpenFileDir();
	static void setRecentOpenFileDir(const QString &dir);
	static QString recentSaveFileDir();
	static void setRecentSaveFileDir(const QString &dir);
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_FILEDIALOG_H
