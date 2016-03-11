#ifndef QF_QMLWIDGETS_HTMLVIEWWIDGET_H
#define QF_QMLWIDGETS_HTMLVIEWWIDGET_H

#include "qmlwidgetsglobal.h"
#include "framework/dialogwidget.h"
#include "framework/ipersistentoptions.h"

namespace qf {
namespace qmlwidgets {

namespace Ui {
class HtmlViewWidget;
}

class QFQMLWIDGETS_DECL_EXPORT HtmlViewWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit HtmlViewWidget(QWidget *parent = 0);
	~HtmlViewWidget() Q_DECL_OVERRIDE;

	//QString htmlText();
	void setHtmlText(const QString &html_text);

private:
	ActionMap createActions() Q_DECL_OVERRIDE;
	void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg) Q_DECL_OVERRIDE;
private:
	Ui::HtmlViewWidget *ui;
};


} // namespace qmlwidgets
} // namespace qf
#endif // QF_QMLWIDGETS_HTMLVIEWWIDGET_H
