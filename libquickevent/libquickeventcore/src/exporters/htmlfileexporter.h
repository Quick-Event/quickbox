#pragma once

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>
#include <qf/core/sql/connection.h>

#include <QObject>

namespace qf { namespace core { namespace sql { class Query; } } }

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT HtmlFileExporter : public QObject
{
	Q_OBJECT

	QF_PROPERTY_IMPL(QString, o, O, utDir)
	QF_PROPERTY_IMPL(QString, r, R, eportTitle)
	QF_PROPERTY_IMPL(QString, r, R, eportDir)
	QF_PROPERTY_IMPL2(int, c, C, urrentStage, 0)
	QF_PROPERTY_IMPL(QString, c, C, lassesLike)
	QF_PROPERTY_IMPL(QString, c, C, lassesNotLike)

public:
	explicit HtmlFileExporter(QObject *parent = nullptr);
	void setSqlConnection(const qf::core::sql::Connection &c) {m_sqlConnection = c;}
	void generateHtml();
protected:
	qf::core::sql::Connection sqlConnection() {return m_sqlConnection;}
	qf::core::sql::Query execSql(const QString &query_str);
	QVariantMap eventInfo();
	void exportClasses();
	virtual void exportClass(int class_id, const QVariantList &class_links) = 0;
	QString normalizeClassName(const QString class_name);
private:
	QVariantMap m_eventInfo;
	qf::core::sql::Connection m_sqlConnection;
};

}}}
