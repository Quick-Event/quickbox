#ifndef TABLE_H
#define TABLE_H

#include <QFrame>

class Table : public QFrame
{
	Q_OBJECT
private:
	using Super = QFrame;
public:
	Table(QWidget *parent);
protected:
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private:
	void updateRowCount();
private:
	QTimer *m_updateRowCountTimer;
	int m_rowCount = 0;
	int m_columnCount = 2;
	QSize m_cellSize;
};

#endif // TABLE_H
