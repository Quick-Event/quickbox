#ifndef QF_CORE_QML_SQLCONNECTION_H
#define QF_CORE_QML_SQLCONNECTION_H

#include <qf/core/sql/connection.h>

#include <QObject>
#include <QSqlDatabase>
#include <QPointer>

namespace qf {
namespace core {
namespace qml {

class SqlQuery;
class SqlQueryBuilder;

class SqlConnection : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString driverName READ driverName)
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
	Q_PROPERTY(QString defaultConnectionName READ defaultConnectionName FINAL)
	Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
	Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
	Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName NOTIFY databaseNameChanged)
	Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
	Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)
public:
	explicit SqlConnection(QObject *parent = 0);
	~SqlConnection() Q_DECL_OVERRIDE;
public:
	QString connectionName() {return m_sqlConnection.connectionName();}
	void setConnectionName(const QString &n);
	Q_SIGNAL void connectionNameChanged();

	QString defaultConnectionName() const;

	QString hostName() {return m_sqlConnection.hostName();}
	void setHostName(const QString &n);
	Q_SIGNAL void hostNameChanged();

	QString userName() {return m_sqlConnection.userName();}
	void setUserName(const QString &n);
	Q_SIGNAL void userNameChanged();

	QString password() const {return m_sqlConnection.password();}
	void setPassword(QString n);
	Q_SIGNAL void passwordChanged();

	QString databaseName() {return m_sqlConnection.databaseName();}
	void setDatabaseName(const QString &n);
	Q_SIGNAL void databaseNameChanged();

	int port() {return m_sqlConnection.port();}
	void setPort(int n);
	Q_SIGNAL void portChanged();

	bool isOpen() {return m_sqlConnection.isOpen();}
	Q_SIGNAL void isOpenChanged();

	QString driverName();

    Q_INVOKABLE QString errorString();

	/// reload m_sqlDatabase according to its connectionName
	/// call it when driver for its connectionName is changed
	//Q_SLOT void reloadConnection();
	Q_INVOKABLE bool open();
	Q_INVOKABLE void close();
	Q_INVOKABLE bool transaction();
	Q_INVOKABLE bool commit();
	Q_INVOKABLE bool rollback();
	Q_INVOKABLE qf::core::qml::SqlQuery* query();
	//Q_INVOKABLE qf::core::qml::SqlQuery* exec(const QString &query_str);
	//Q_INVOKABLE qf::core::qml::SqlQuery* exec(qf::core::qml::SqlQueryBuilder *qb);
private:
	qf::core::qml::SqlQuery* createQuery();
private:
	qf::core::sql::Connection m_sqlConnection;
	QString m_defaultConnectionName;
	QPointer<SqlQuery> m_sqlQuery;
};

}}}

#endif // QF_CORE_QML_SQLCONNECTION_H
