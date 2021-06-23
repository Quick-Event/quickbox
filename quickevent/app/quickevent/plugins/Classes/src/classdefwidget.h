#ifndef CLASSDEFWIDGET_H
#define CLASSDEFWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace Ui {
class ClassDefWidget;
}

class ClassDefWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	ClassDefWidget(QWidget *parent = nullptr);
	~ClassDefWidget() Q_DECL_OVERRIDE;

	bool load(const QVariant &id = QVariant(), int mode = qf::core::model::DataDocument::ModeEdit) Q_DECL_OVERRIDE;
private:
	Ui::ClassDefWidget *ui;
};

#endif // CLASSDEFWIDGET_H
