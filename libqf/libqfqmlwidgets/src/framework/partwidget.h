#ifndef QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"

#include <qf/core/utils.h>

#include <QWidget>
#include <QQmlListProperty>
#include <QQmlParserStatus>

class QHBoxLayout;

namespace qf {
namespace qmlwidgets {
class Frame;
class MenuBar;
class ToolBar;
namespace dialogs {
namespace internal {
class CaptionFrame;
}
}
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT PartWidget : public QWidget, public IPersistentSettings, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	/// url or path to part icon
	Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
	Q_PROPERTY(QString featureId READ featureId WRITE setFeatureId NOTIFY featureIdChanged FINAL)
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
	/// attachedObjects is a workaround for https://github.com/fvacek/quickbox/issues/2
	Q_PROPERTY(QQmlListProperty<QObject> attachedObjects READ attachedObjects)
private:
	typedef QWidget Super;
public:
	explicit PartWidget(QWidget *parent = 0);
	~PartWidget() Q_DECL_OVERRIDE;
public:
	QString title() const { return m_title; }
	void setTitle(QString arg);
	Q_SIGNAL void titleChanged(const QString &s);

	QString iconSource() const { return m_iconSource; }
	void setIconSource(QString arg);
	Q_SIGNAL void iconSourceChanged(const QString &s);

	QString featureId() const { return m_featureId; }
	void setFeatureId(QString id);
	Q_SIGNAL void featureIdChanged(const QString &s);

	QF_PROPERTY_BOOL_IMPL(a, A, ctive)

	QQmlListProperty<QObject> attachedObjects();

	Q_SLOT void loadPersistentSettings();

	QIcon createIcon();
	Q_SLOT void updateCaptionFrame();

	Q_INVOKABLE MenuBar* menuBar();
	Q_INVOKABLE ToolBar* addToolBar();
protected:
	QQmlListProperty<QWidget> widgets();
	Frame* centralFrame();
private:
	Q_SLOT void savePersistentSettings();
	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
private:
	MenuBar *m_menuBar = nullptr;
	QList<ToolBar*> m_toolBars;
	QHBoxLayout *m_toolBarsLayout = nullptr;
	qf::qmlwidgets::dialogs::internal::CaptionFrame *m_captionFrame;
	Frame *m_centralFrame;
	QString m_title;
	QString m_iconSource;
	QString m_featureId;
	QList<QObject*> m_attachedObjects;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H
