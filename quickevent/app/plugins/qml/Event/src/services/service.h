#ifndef SERVICES_SERVICE_H
#define SERVICES_SERVICE_H

#include <qf/core/utils.h>

#include <QtCore/QObject>

namespace qf { namespace qmlwidgets { namespace framework { class DialogWidget; }}}

namespace services {

class Service : public QObject
{
	Q_OBJECT

	QF_PROPERTY_IMPL(QString, s, S, tatusMessage)
public:
	enum class Status { Unknown, Stopped, Running};
public:
	explicit Service(QObject *parent = nullptr);

	QString name() const {return objectName();}

	virtual void loadConfig();
	virtual void run();
	virtual void stop();
	void restart()
	{
		stop();
		loadConfig();
		run();
	}

	void setRunning(bool on);

	virtual void showDetail();

	Status status() const {return m_status;}
	Q_SIGNAL void statusChanged(Status new_status);

	static void addService(Service *service);
	static int serviceCount() {return m_services.count();}
	static Service* serviceAt(int ix);
	//Q_SIGNAL void serviceCountChanged(int new_count);
protected:
	void setStatus(Status st)
	{
		if(st == status())
			return;
		m_status = st;
		emit statusChanged(st);
	}

	virtual qf::qmlwidgets::framework::DialogWidget *createDetailWidget();
private:
	Status m_status = Status::Unknown;
	static QList<Service*> m_services;
};

} // namespace services

#endif // SERVICES_SERVICE_H
