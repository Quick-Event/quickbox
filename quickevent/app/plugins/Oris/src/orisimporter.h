#ifndef ORISIMPORTER_H
#define ORISIMPORTER_H

#include <QObject>

#include <functional>

class QUrl;
class QJsonDocument;

namespace qf { namespace core { namespace network { class NetworkAccessManager; }}}

class OrisImporter : public QObject
{
	Q_OBJECT
public:
	explicit OrisImporter(QObject *parent = nullptr);

	Q_INVOKABLE void chooseAndImport();
	Q_INVOKABLE void syncCurrentEventEntries();
	Q_INVOKABLE void syncRelaysEntries(int oris_id);
	void importEvent(int event_id);
	void importEventOrisEntries(int event_id);
	Q_INVOKABLE void importRegistrations();
	Q_INVOKABLE void importClubs();

	static void saveJsonBackup(const QString &fn, const QJsonDocument &jsd);
	void getJsonAndProcess(const QUrl &url, QObject *context, std::function<void (const QJsonDocument &data)> process_call_back);
	void getTextAndProcess(const QUrl &url, QObject *context, std::function<void (const QByteArray &)> process_call_back);
private:
	qf::core::network::NetworkAccessManager *networkAccessManager();
private:
	qf::core::network::NetworkAccessManager *m_networkAccessManager = nullptr;
};

#endif // ORISIMPORTER_H
