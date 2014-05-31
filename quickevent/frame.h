#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QQmlListProperty>

class QBoxLayout;

class Frame : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets FINAL)
	Q_PROPERTY(LayoutType layoutType READ layoutType WRITE setLayoutType NOTIFY layoutTypeChanged)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_ENUMS(LayoutType)
private:
	typedef QFrame Super;
public:
	enum LayoutType {LayoutHorizontal, LayoutVertical};
public:
	explicit Frame(QWidget *parent = 0);
public:
	LayoutType layoutType() const;
	void setLayoutType(LayoutType ly);
	Q_SIGNAL void layoutTypeChanged(LayoutType ly);
signals:

public slots:
private:
	QQmlListProperty<QWidget> widgets();

	static void addWidgetFunction(QQmlListProperty<QWidget> *listProperty, QWidget *value);
	static QWidget * widgetAtFunction(QQmlListProperty<QWidget> *listProperty, int index);
	static void removeAllWidgetsFunction(QQmlListProperty<QWidget> *listProperty);
	static int countWidgetsFunction(QQmlListProperty<QWidget> *listProperty);

	void add(QWidget *control);
	QWidget* at(int index) const;
	void removeAll();
	int count() const;
private:
	//QBoxLayout *m_layout;
};

#endif // FRAME_H
