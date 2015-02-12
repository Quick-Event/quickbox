#ifndef CARDREADOUTWIDGET_H
#define CARDREADOUTWIDGET_H

#include <QFrame>

namespace Ui {
	class CardReadoutWidget;
}

class CardReadoutWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CardReadoutWidget(QWidget *parent = 0);
	~CardReadoutWidget() Q_DECL_OVERRIDE;
private:
	Ui::CardReadoutWidget *ui;
};

#endif // CARDREADOUTWIDGET_H
