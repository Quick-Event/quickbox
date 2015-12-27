#ifndef EDITCODESWIDGET_H
#define EDITCODESWIDGET_H

#include <QWidget>

namespace Ui {
class EditCodesWidget;
}

class EditCodesWidget : public QWidget
{
	Q_OBJECT

public:
	explicit EditCodesWidget(QWidget *parent = 0);
	~EditCodesWidget();

private:
	Ui::EditCodesWidget *ui;
};

#endif // EDITCODESWIDGET_H
