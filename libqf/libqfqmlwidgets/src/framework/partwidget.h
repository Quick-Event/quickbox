#ifndef QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"

#include <QWidget>
#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace qf {
namespace qmlwidgets {
class Frame;
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT PartWidget : public QWidget, public IPersistentSettings, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QString featureId READ featureId WRITE setFeatureId NOTIFY featureIdChanged FINAL)
private:
	typedef QWidget Super;
public:
	explicit PartWidget(QWidget *parent = 0);
	~PartWidget() Q_DECL_OVERRIDE;
public:
	QString title() const { return m_title; }
	void setTitle(QString arg);
	Q_SIGNAL void titleChanged(const QString &s);

	QString featureId() const { return m_featureId; }
	void setFeatureId(QString id);
	Q_SIGNAL void featureIdChanged(const QString &s);

	//Q_SLOT virtual bool canActivate(bool active_on);
protected:
	QQmlListProperty<QWidget> widgets();
private:
	Q_SLOT void savePersistentSettings();
	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
private:
	Frame *m_centralFrame;
	QString m_title;
	QString m_featureId;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_PARTWIDGET_H
