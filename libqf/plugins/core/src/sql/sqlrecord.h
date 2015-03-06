#ifndef QF_CORE_QML_SQLRECORD_H
#define QF_CORE_QML_SQLRECORD_H

#include <QObject>
#include <QSqlRecord>
#include <QStringList>

namespace qf {
namespace core {
namespace qml {

class SqlRecord : public QObject
{
	Q_OBJECT
public:
	explicit SqlRecord(QObject *parent = 0);
	~SqlRecord() Q_DECL_OVERRIDE;

	void setRecord(const QSqlRecord &rec);
public:
	Q_INVOKABLE int count() {return m_record.count();}
	Q_INVOKABLE QString fieldName(int ix) {return m_record.fieldName(ix);}
	Q_INVOKABLE QString shortFieldName(int ix);
	Q_INVOKABLE QStringList fieldNames();
private:
	QSqlRecord m_record;
};

}}}

#endif // QF_CORE_QML_SQLRECORD_H
