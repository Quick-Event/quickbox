#ifndef CELLRENDERER_H
#define CELLRENDERER_H

#include <QSize>
#include <QVector>

class QWidget;
class QPainter;
class QPoint;

class CellRenderer
{
public:
	CellRenderer(const QSize &size, QWidget *widget);

	virtual void draw(const QPoint &position, QWidget *widget) = 0;
protected:
	const QSize m_size;
	int m_cellSpacing;
	double m_fontScale = 1;
	int m_fontAscent;
	int m_fontDescent;
	int m_scaledLetterWidth;
};

class ResultsCellRenderer : public CellRenderer
{
private:
	using Super = CellRenderer;
public:
	ResultsCellRenderer(const QSize &size, QWidget *widget);

	void draw(const QPoint &position, QWidget *widget) Q_DECL_OVERRIDE;
protected:
	enum Column {Position = 0, Name, Registration, Time, Status, ColumnCount};
	QString columnText(Column col);
protected:
	QVector<int> m_cellWidths;
};

#endif // CELLRENDERER_H
