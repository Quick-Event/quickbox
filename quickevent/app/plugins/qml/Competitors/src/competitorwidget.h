#ifndef COMPETITORWIDGET_H
#define COMPETITORWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace Ui {
class CompetitorWidget;
}

class CompetitorWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit CompetitorWidget(QWidget *parent = 0);
	~CompetitorWidget() Q_DECL_OVERRIDE;
private:
	Ui::CompetitorWidget *ui;
};

#endif // COMPETITORWIDGET_H
