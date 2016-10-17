#ifndef RESULTSCELL_H
#define RESULTSCELL_H

#include <QWidget>

namespace Ui {
class ResultsCell;
}

class ResultsCell : public QWidget
{
	Q_OBJECT

public:
	explicit ResultsCell(QWidget *parent = 0);
	~ResultsCell();

private:
	Ui::ResultsCell *ui;
};

#endif // RESULTSCELL_H
