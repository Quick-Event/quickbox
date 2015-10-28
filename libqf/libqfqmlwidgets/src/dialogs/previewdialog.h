#ifndef QF_QMLWIDGETS_DIALOGS_PREVIEWDIALOG_H
#define QF_QMLWIDGETS_DIALOGS_PREVIEWDIALOG_H

#include "../qmlwidgetsglobal.h"
#include "dialog.h"
#include "../framework/ipersistentsettings.h"

#include <QDialog>

class QUrl;
class QTextEdit;

namespace Ui {
	class PreviewDialog;
}

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT PreviewDialog : public Dialog
{
	Q_OBJECT
private:
	typedef Dialog Super;
public:
	PreviewDialog(QWidget *parent = 0);
	~PreviewDialog() Q_DECL_OVERRIDE;
public:
	QString text();
	void setText(const QString& content, const QString &suggested_file_name = QString());
	void setFile(const QString& file_name, const QString &codec_name = QString());
	void setUrl(const QUrl &url, const QString &codec_name = QString());

	int exec() Q_DECL_OVERRIDE {return Super::exec();}
	int exec(const QString &content, const QString &suggested_file_name = QString(), const QString &persistent_data_id = QString());

	QTextEdit* editor();
	//QFTextViewWidget* textViewWidget() {return f_editor;}

	static int exec(QWidget *parent, const QString &content, const QString &suggested_file_name = QString(), const QString &persistent_data_id = QString());
	static int exec(QWidget *parent, const QUrl &url, const QString &persistent_data_id = QString(), const QString &codec_name = QString());
private:
	Ui::PreviewDialog *ui;
	QString m_suggestedFileName;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_PREVIEWDIALOG_H
