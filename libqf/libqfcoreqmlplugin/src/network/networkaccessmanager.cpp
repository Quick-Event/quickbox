#include "networkaccessmanager.h"
#include "networkreply.h"

#include <qf/core/log.h>

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

using namespace qf::core::qml;

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
	Super(parent)
{
}

NetworkAccessManager::~NetworkAccessManager()
{
}

NetworkReply *NetworkAccessManager::get(const QUrl &url)
{
	NetworkReply *ret = new NetworkReply();
	QNetworkRequest rq(url);
	QNetworkReply *repl = Super::get(rq);
	ret->setReply(repl);
	return ret;
}

