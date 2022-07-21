#pragma once

#include "../quickeventcoreglobal.h"

#include <qf/core/utils.h>
#include <qf/core/sql/connection.h>

#include <QObject>

namespace qf { namespace core { namespace sql { class Query; } } }

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT FileExporter : public QObject
{
	Q_OBJECT
public:
	QF_PROPERTY_IMPL(QString, o, O, utDir)
	QF_PROPERTY_IMPL(QString, r, R, eportTitle)
	QF_PROPERTY_IMPL(QString, r, R, eportDir)
	QF_PROPERTY_IMPL2(int, c, C, urrentStage, 0)
	QF_PROPERTY_IMPL(QString, c, C, lassesLike)
	QF_PROPERTY_IMPL(QString, c, C, lassesNotLike)
public:
	explicit FileExporter(QObject *parent = nullptr);
	void setSqlConnection(const qf::core::sql::Connection &c) {m_sqlConnection = c;}
protected:
	qf::core::sql::Connection sqlConnection() {return m_sqlConnection;}
	qf::core::sql::Query execSql(const QString &query_str);
	QVariantMap eventInfo();
	QString normalizeClassName(const QString class_name);
	void prepareExport();
private:
	QVariantMap m_eventInfo;
	qf::core::sql::Connection m_sqlConnection;
};

}}}
