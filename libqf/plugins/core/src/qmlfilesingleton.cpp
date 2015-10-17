#include "qmlfilesingleton.h"

#include <qf/core/collator.h>
#include <qf/core/log.h>
#include <qf/core/utils/htmlutils.h>

#include <QFile>
#include <QDir>
#include <QQmlEngine>
#include <QJSEngine>
#include <QDesktopServices>

using namespace qf::core::qml;

QmlFileSingleton::QmlFileSingleton(QObject *parent)
	: QObject(parent)
{
}

QmlFileSingleton::~QmlFileSingleton()
{
}

QObject *QmlFileSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)

	auto *s = new QmlFileSingleton(engine);
	return s;
}

bool QmlFileSingleton::write(const QString &file_path, const QString &content)
{
	QFile f(file_path);
	if(!f.open(QFile::WriteOnly)) {
		qfWarning() << "Cannot open file" << file_path << "for write.";
		return false;
	}
	auto l = f.write(content.toUtf8());
	return l >= 0;
}

bool QmlFileSingleton::writeHtml(const QString &file_path, const QVariant &body_list, const QVariantMap &options)
{
	QString str;
	QVariantList html_lst = body_list.toList();
	if(html_lst.isEmpty()) {
		str = body_list.toString();
	}
	else {
		qf::core::utils::HtmlUtils::FromHtmlListOptions opts(options);
		str = qf::core::utils::HtmlUtils::fromHtmlList(html_lst, opts);
	}
	return write(file_path, str);
}

bool QmlFileSingleton::writeXml(const QString &file_path, const QVariant &body_list, const QVariantMap &options)
{
	QString str;
	QVariantList xml_lst = body_list.toList();
	if(xml_lst.isEmpty()) {
		str = body_list.toString();
	}
	else {
		qf::core::utils::HtmlUtils::FromXmlListOptions opts(options);
		str = qf::core::utils::HtmlUtils::fromXmlList(xml_lst, opts);
	}
	return write(file_path, str);
}

QString QmlFileSingleton::toAscii7(const QString &s, bool to_lower)
{
	return qf::core::Collator::toAscii7(QLocale::Czech, s, to_lower);
}

QString QmlFileSingleton::tempPath()
{
	return QDir::tempPath();
}

bool QmlFileSingleton::mkpath(const QString &absolute_dir_path)
{
	QDir dir(absolute_dir_path);
	return dir.mkpath(QStringLiteral("."));
}

QUrl QmlFileSingleton::toUrl(const QString &file_path)
{
	return QUrl::fromLocalFile(file_path);
}

void QmlFileSingleton::openUrl(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

