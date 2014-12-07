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
	explicit FileDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
public:
	static QString getOpenFileName(QWidget *parent = 0, const QString &caption = QString(),
				const QString &dir = QString(), const QString &filter = QString(),
				QString *selectedFilter = 0, QFileDialog::Options options = 0);
	static QStringList getOpenFileNames(QWidget *parent = 0, const QString &caption = QString(),
				const QString &dir = QString(), const QString &filter = QString(),
				QString *selectedFilter = 0, QFileDialog::Options options = 0);
	static QString getSaveFileName(QWidget * parent = 0, const QString & caption = QString(),
				const QString & dir = QString(), const QString & filter = QString(),
				QString * selectedFilter = 0, QFileDialog::Options options = 0);
	static QString getExistingDirectory(QWidget * parent = 0, const QString & caption = QString(),
				const QString & dir = QString(), Options options = ShowDirsOnly);
private:
	static QString s_recentOpenFileDir;
	static QString s_recentSaveFileDir;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_FILEDIALOG_H
