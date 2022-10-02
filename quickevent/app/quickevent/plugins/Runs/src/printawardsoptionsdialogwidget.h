#ifndef PRINTAWARDSOPTIONSDIALOGWIDGET_H
#define PRINTAWARDSOPTIONSDIALOGWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

#include <QDialog>

namespace Ui {
class PrintAwardsOptionsDialogWidget;
}

class PrintAwardsOptionsDialogWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::framework::DialogWidget;
public:
	explicit PrintAwardsOptionsDialogWidget(QWidget *parent = 0);
	~PrintAwardsOptionsDialogWidget();

	QVariantMap printOptions() const;
	void setPrintOptions(const QVariantMap &opts);
private:
	Ui::PrintAwardsOptionsDialogWidget *ui;
};

#endif
