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

	void chooseAndImport();
	void importEvent(int event_id);
	void syncCurrentEventEntries();
	void importRegistrations();
	void importClubs();

	static void saveJsonBackup(const QString &fn, const QJsonDocument &jsd);
	void getJsonAndProcess(const QUrl &url, QObject *context, std::function<void (const QJsonDocument &data)> process_call_back);
	void getTextAndProcess(const QUrl &url, QObject *context, std::function<void (const QByteArray &)> process_call_back);
protected:
	Q_SIGNAL void syncEventEntriesSuccess();
	Q_SIGNAL void importRegistrationsSuccess();
	Q_SIGNAL void importClubsSuccess();

	void syncEventEntries(int event_id);
	void syncRelaysEntries(int event_id);
private:
	qf::core::network::NetworkAccessManager *networkAccessManager();
private:
	qf::core::network::NetworkAccessManager *m_networkAccessManager = nullptr;
};

#endif // ORISIMPORTER_H
