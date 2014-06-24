#ifndef QF_QMLWIDGETS_FRAME_H
#define QF_QMLWIDGETS_FRAME_H

#include "qmlwidgetsglobal.h"

#include <QFrame>
#include <QQmlListProperty>

class QBoxLayout;

namespace qf {
namespace qmlwidgets {

class GridLayoutProperties;

class QFQMLWIDGETS_DECL_EXPORT Frame : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_PROPERTY(LayoutType layoutType READ layoutType WRITE setLayoutType NOTIFY layoutTypeChanged)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_ENUMS(LayoutType)
	Q_PROPERTY(qf::qmlwidgets::GridLayoutProperties* gridLayoutProperties READ gridLayoutProperties WRITE setGridLayoutProperties)
private:
	typedef QFrame Super;
public:
	enum LayoutType {LayoutInvalid, LayoutHorizontal, LayoutVertical, LayoutGrid, LayoutForm};
public:
	explicit Frame(QWidget *parent = 0);
public:
	LayoutType layoutType() const;
	void setLayoutType(LayoutType ly);
	Q_SIGNAL void layoutTypeChanged(LayoutType ly);
	QQmlListProperty<QWidget> widgets();
signals:

public slots:
private:
	static void addWidgetFunction(QQmlListProperty<QWidget> *listProperty, QWidget *value);
	static QWidget* widgetAtFunction(QQmlListProperty<QWidget> *listProperty, int index);
	static void removeAllWidgetsFunction(QQmlListProperty<QWidget> *listProperty);
	static int countWidgetsFunction(QQmlListProperty<QWidget> *listProperty);

	void add(QWidget *control);
	QWidget* at(int index) const;
	void removeAll();
	int count() const;

	GridLayoutProperties* gridLayoutProperties() {return m_gridLayoutProperties;}
	void setGridLayoutProperties(GridLayoutProperties *props);
	//Q_SIGNAL void gridLayoutPropertiesChanged();
	//Q_SLOT void initLayout();
	void addToLayout(QWidget *widget);
	void createLayout(LayoutType layout_type);
private:
	LayoutType m_layoutType;
	GridLayoutProperties *m_gridLayoutProperties;
	QList<QWidget*> m_childWidgets;
	int m_currentLayoutRow;
	int m_currentLayoutColumn;
};

}
}

#endif // FRAME_H
