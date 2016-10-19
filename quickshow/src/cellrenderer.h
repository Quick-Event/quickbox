#ifndef CELLRENDERER_H
#define CELLRENDERER_H

#include <QSize>
#include <QVector>
#include <QVariantMap>
#include <QCoreApplication>

class QWidget;
class QPainter;
class QPoint;

class CellRenderer
{
public:
	CellRenderer(const QSize &size, QWidget *widget);

	virtual void draw(const QPoint &position, const QVariantMap &data, QWidget *widget) = 0;
protected:
	const QSize m_size;
	int m_cellSpacing;
	double m_fontScale = 1;
	int m_fontAscent;
	int m_fontDescent;
	int m_scaledLetterWidth;

	struct CellAttribute
	{
		int width = 0;
		int alignment = 0;
		CellAttribute(int w = 0, int a = 0) : width(w), alignment(a) {}
	};
	QVector<CellAttribute> m_cellAttributes;
};

class ClassCellRenderer : public CellRenderer
{
	Q_DECLARE_TR_FUNCTIONS(ClassCellRenderer)
private:
	using Super = CellRenderer;
public:
	ClassCellRenderer(const QSize &size, QWidget *widget);

	void draw(const QPoint &position, const QVariantMap &data, QWidget *widget) Q_DECL_OVERRIDE;
protected:
	enum Column {Name = 0, Info, ColumnCount};
	QString columnText(Column col, const QVariantMap &data);
protected:
};

class ResultsCellRenderer : public CellRenderer
{
private:
	using Super = CellRenderer;
public:
	ResultsCellRenderer(const QSize &size, QWidget *widget);

	void draw(const QPoint &position, const QVariantMap &data, QWidget *widget) Q_DECL_OVERRIDE;
protected:
	enum Column {Position = 0, Name, Registration, Time, Status, ColumnCount};
	QString columnText(Column col, const QVariantMap &data);
};

#endif // CELLRENDERER_H
