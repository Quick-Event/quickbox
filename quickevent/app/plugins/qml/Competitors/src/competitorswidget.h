#ifndef COMPETITORSWIDGET_H
#define COMPETITORSWIDGET_H

#include <QFrame>

namespace Ui {
class CompetitorsWidget;
}

class CompetitorsWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CompetitorsWidget(QWidget *parent = 0);
	~CompetitorsWidget() Q_DECL_OVERRIDE;

private:
	Ui::CompetitorsWidget *ui;
};

#endif // COMPETITORSWIDGET_H
