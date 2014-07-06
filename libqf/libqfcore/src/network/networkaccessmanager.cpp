#include "networkaccessmanager.h"
#include "networkreply.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

using namespace qf::core::network;

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
	QNetworkAccessManager(parent)
{
}

NetworkReply *NetworkAccessManager::get(const QUrl &content_url)
{
	QNetworkRequest rq;
	rq.setUrl(content_url);
	rq.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
}
