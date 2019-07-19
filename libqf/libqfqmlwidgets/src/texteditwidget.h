#ifndef QF_QMLWIDGETS_TEXTEDITWIDGET_H
#define QF_QMLWIDGETS_TEXTEDITWIDGET_H

#include "qmlwidgetsglobal.h"
#include "framework/dialogwidget.h"
#include "framework/ipersistentoptions.h"

#include <qf/core/utils.h>

class QTextEdit;

namespace qf {
namespace qmlwidgets {

namespace dialogs {
class Dialog;
}
namespace Ui {
class TextEditWidget;
}

class QFQMLWIDGETS_DECL_EXPORT TextEditWidget : public qf::qmlwidgets::framework::DialogWidget //, public qf::qmlwidgets::framework::IPersistentOptions
{
	Q_OBJECT
	Q_PROPERTY(QString codecName READ codecName WRITE setCodecName NOTIFY codecNameChanged)
	Q_PROPERTY(QString suggestedFileName READ suggestedFileName WRITE setSuggestedFileName NOTIFY suggestedFileNameChanged)
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	TextEditWidget(QWidget *parent = nullptr);
	~TextEditWidget() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL2(QString, c, C, odecName, QStringLiteral("System"))
	QF_PROPERTY_IMPL2(QString, s, S, uggestedFileName, QStringLiteral("new.txt"))

	//QVariant persistentOptions() Q_DECL_OVERRIDE { return QVariant();}
	//void setPersistentOptions(const QVariant &opts) Q_DECL_OVERRIDE {Q_UNUSED(opts)}
public:
	QString text();
	void setText(const QString &doc);
	//void setUrl(QFile &url, const QString &codec_name = QString());
	//void setFile(const QString &file_name, const QString &codec_name = QString());
	QString fileName() {return suggestedFileName();}

	Q_SLOT void save();
	Q_SLOT void saveAs();
	Q_SLOT void print();

protected:
	QTextEdit* editor();
	void save_helper(const QString &file_name);
private:
	ActionMap createActions() Q_DECL_OVERRIDE;
	void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg) Q_DECL_OVERRIDE;

	Q_SLOT void wrapLines(bool checked);
private:
	Ui::TextEditWidget *ui;
	//QFAction::ActionList toolBarActions;
	//QFAction::ActionList f_actions;
	/*
	void createToolBar();
	void createActions();
	QFAction::ActionList& actionListRef() {return f_actions;}
	QFAction::ActionList actionList() const {
		return ((QFTextViewWidget*)this)->actionListRef();
	}
public:
	virtual QFPart::ToolBarList createToolBars();
	*/
};

}}

#endif
