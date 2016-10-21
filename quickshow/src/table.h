#ifndef TABLE_H
#define TABLE_H

#include <QFrame>

class Model;

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
	Model *model();
private:
	QTimer *m_updateRowCountTimer;
	int m_rowCount = 0;
	int m_columnCount = 2;
	QSize m_cellSize;
	Model *m_model;
	QTimer *m_scrollTimer = nullptr;
};

#endif // TABLE_H
