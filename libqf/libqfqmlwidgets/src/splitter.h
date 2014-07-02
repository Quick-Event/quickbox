#ifndef QF_QMLWIDGETS_SPLITTER_H
#define QF_QMLWIDGETS_SPLITTER_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

#include <QSplitter>
#include <QQmlListProperty>

class QWidget;

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Splitter : public QSplitter, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
public:
	explicit Splitter(QWidget *parent = 0);
	~Splitter() Q_DECL_OVERRIDE;
public:
	QQmlListProperty<QWidget> widgets();
private:
	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
private:
	static void addWidgetFunction(QQmlListProperty<QWidget> *listProperty, QWidget *widget);
	static QWidget* widgetAtFunction(QQmlListProperty<QWidget> *listProperty, int index);
	static void removeAllWidgetsFunction(QQmlListProperty<QWidget> *listProperty);
	static int countWidgetsFunction(QQmlListProperty<QWidget> *listProperty);
};

}}

#endif
