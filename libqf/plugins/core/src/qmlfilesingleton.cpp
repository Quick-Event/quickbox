#include "qmlfilesingleton.h"

#include <qf/core/log.h>
#include <qf/core/utils/htmlutils.h>

#include <QFile>
#include <QQmlEngine>
#include <QJSEngine>

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

