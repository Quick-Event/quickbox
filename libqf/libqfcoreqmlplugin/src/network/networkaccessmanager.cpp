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
	qfLogFuncFrame() << this;
}

NetworkReply *NetworkAccessManager::get(const QUrl &url)
{
	NetworkReply *ret = new NetworkReply();
	QNetworkRequest rq(url);
	//rq.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
	QNetworkReply *repl = Super::get(rq);
	ret->setReply(repl);
	return ret;
}

