#include "httpconnection.h"

#include "application.h"
#include "appclioptions.h"

#include <qf/core/log.h>
#include <qf/core/utils/htmlutils.h>

#include <QDir>

HttpConnection::HttpConnection(QTcpSocket *sock, QObject *parent)
	: QObject(parent)
	, m_socket(sock)
{
	connect(sock, &QAbstractSocket::disconnected, sock, &QObject::deleteLater);
	connect(sock, &QAbstractSocket::readyRead, this, &HttpConnection::onReadyRead);
}

HttpConnection::~HttpConnection()
{
	delete m_socket;
}

void HttpConnection::onReadyRead()
{
	while(m_socket->canReadLine()) {
		QByteArray line = m_socket->readLine();
		qfInfo() << line;
		if(line.startsWith("GET ")) {
			m_getPath = line.mid(4);
			int ix = m_getPath.indexOf("HTTP");
			m_getPath = m_getPath.mid(0, ix - 1);
		}
		else if(line == "\r\n") {
			processRequest();
		}
	}
}

void HttpConnection::processRequest()
{
	if(!m_getPath.isEmpty()) {
		if(!m_getPath.startsWith('/'))
			m_getPath = '/' + m_getPath;
		Application *app = Application::instance();
		AppCliOptions *cliopts = app->cliOptions();
		QString html_dir = cliopts->htmlDir();
		QFileInfo fi(html_dir + m_getPath);
		qfInfo() << "GET" << m_getPath << "abs:" << fi.absoluteFilePath();
		if(fi.isDir()) {
			if(!m_getPath.endsWith('/'))
				m_getPath += '/';
			QVariantList html_body = QVariantList() << QStringLiteral("body");
			QDir dir(fi.absoluteFilePath());
			QVariantList class_links;
			qfInfo() << "dir of:" << dir.absolutePath();
			for(const QString &fn : dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
				class_links.insert(class_links.length(), QVariantList{"a", QVariantMap{{"href", m_getPath + fn}}, fn});
			}
			html_body.insert(html_body.length(), QVariantList{"p"} << class_links);

			qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
			opts.setDocumentTitle(tr("Dir list %1").arg(m_getPath));
			QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
			responseOK(html.toUtf8());
		}
		else {
			QFile f(fi.absoluteFilePath());
			if(f.open(QFile::ReadOnly)) {
				QByteArray ba = f.readAll();
				responseOK(ba);
			}
		}
	}
	m_socket->close();
}

void HttpConnection::responseOK(const QByteArray &message)
{
	m_socket->write(QByteArray("HTTP/1.1 200 OK\r\n"));
	m_socket->write(QByteArray("Content-Type: text/html\r\n"));
	m_socket->write(QByteArray("\r\n"));
	m_socket->write(message);
}
