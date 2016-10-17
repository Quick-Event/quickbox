#ifndef CELLSTARTLIST_H
#define CELLSTARTLIST_H

#include <QWidget>

namespace Ui {
class CellStartList;
}

class CellStartList : public QWidget
{
	Q_OBJECT

public:
	explicit CellStartList(QWidget *parent = 0);
	~CellStartList();

private:
	Ui::CellStartList *ui;
};

#endif // CELLSTARTLIST_H
