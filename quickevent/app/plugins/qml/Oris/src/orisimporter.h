#ifndef ORISIMPORTER_H
#define ORISIMPORTER_H

#include <QObject>

#include <functional>

namespace qf { namespace core { namespace network { class NetworkAccessManager; }}}

class OrisImporter : public QObject
{
	Q_OBJECT
public:
	explicit OrisImporter(QObject *parent = 0);

	Q_INVOKABLE void chooseAndImport();
	Q_INVOKABLE  void syncCurrentEventEntries();
	void importEvent(int event_id);
	void importEventOrisEntries(int event_id);
	Q_INVOKABLE void importRegistrations();
	Q_INVOKABLE void importClubs();
private:
	void getJsonAndProcess(const QUrl &url, std::function<void (const QJsonDocument &data)> process_call_back);
	qf::core::network::NetworkAccessManager *networkAccessManager();
private:
	qf::core::network::NetworkAccessManager *m_networkAccessManager = nullptr;
};

#endif // ORISIMPORTER_H
