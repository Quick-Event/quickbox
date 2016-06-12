#ifndef CODECLASSRESULTSWIDGET_H
#define CODECLASSRESULTSWIDGET_H

#include <QWidget>

class QJsonObject;

namespace Ui {
class CodeClassResultsWidget;
}

class CodeClassResultsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CodeClassResultsWidget(QWidget *parent = 0);
	~CodeClassResultsWidget() Q_DECL_OVERRIDE;

	QJsonObject saveSetup();
private:
	Ui::CodeClassResultsWidget *ui;
};

#endif // CODECLASSRESULTSWIDGET_H
