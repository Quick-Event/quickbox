#ifndef SERVICES_SERVICE_H
#define SERVICES_SERVICE_H

#include "../eventpluginglobal.h"

#include <qf/core/utils.h>

#include <QObject>

namespace qf { namespace qmlwidgets { namespace framework { class DialogWidget; }}}
namespace Event { class EventPlugin; }

namespace services {

class EVENTPLUGIN_DECL_EXPORT ServiceSettings : public QVariantMap
{
	using Super = QVariantMap;

	QF_VARIANTMAP_FIELD(bool, is, set, AutoStart)

public:
	ServiceSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class EVENTPLUGIN_DECL_EXPORT Service : public QObject
{
	Q_OBJECT

	QF_PROPERTY_IMPL(QString, s, S, tatusMessage)
public:
	enum class Status { Unknown, Stopped, Running};
public:
	explicit Service(const QString &name, QObject *parent = nullptr);
	~Service() override;

	QString name() const {return objectName();}

	virtual void run();
	virtual void stop();
	void restart()
	{
		stop();
		loadSettings();
		run();
	}

	virtual void loadSettings();
	ServiceSettings settings() const {return ServiceSettings(m_settings);}
	void saveSettings();
	void setSettings(const QVariantMap &s);
	Q_SIGNAL void settingsChanged();

	void showDetail(QWidget *parent);

	Status status() const {return m_status;}
	Q_SIGNAL void statusChanged(Status new_status);

	QString settingsGroup() const;

	void setRunning(bool on);

	static void addService(Service *service);
	static int serviceCount() {return m_services.count();}
	static Service* serviceAt(int ix);
	static Service* serviceByName(const QString &service_name);
	//Q_SIGNAL void serviceCountChanged(int new_count);
protected:
	virtual void onEventOpen();
	void setStatus(Status st)
	{
		if(st == status())
			return;
		m_status = st;
		emit statusChanged(st);
	}

	virtual qf::qmlwidgets::framework::DialogWidget *createDetailWidget();
	static Event::EventPlugin *eventPlugin();
protected:
	QVariantMap m_settings;
private:
	Status m_status = Status::Unknown;
	static QList<Service*> m_services;
};

} // namespace services

#endif // SERVICES_SERVICE_H
