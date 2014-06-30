#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QSqlDatabase>

namespace qf {
namespace core {
namespace qml {

class SqlDatabase : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString connectionName READ connectionName WRITE setConnectionName NOTIFY connectionNameChanged)
	Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
	Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
	Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName NOTIFY databaseNameChanged)
	Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
public:
	explicit SqlDatabase(const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection), QObject *parent = 0);
public:
	Q_SLOT QString connectionName() {return m_sqlDatabase.connectionName();}
	Q_SLOT void setConnectionName(const QString &n);
	Q_SIGNAL void connectionNameChanged();

	Q_SLOT QString hostName() {return m_sqlDatabase.hostName();}
	Q_SLOT void setHostName(const QString &n);
	Q_SIGNAL void hostNameChanged();

	Q_SLOT QString userName() {return m_sqlDatabase.userName();}
	Q_SLOT void setUserName(const QString &n);
	Q_SIGNAL void userNameChanged();

	Q_SLOT QString databaseName() {return m_sqlDatabase.databaseName();}
	Q_SLOT void setDatabaseName(const QString &n);
	Q_SIGNAL void databaseNameChanged();

	Q_SLOT int port() {return m_sqlDatabase.port();}
	Q_SLOT void setPort(int n);
	Q_SIGNAL void portChanged();

	Q_INVOKABLE bool open();
private:
	QSqlDatabase m_sqlDatabase;
};

}}}

#endif // SQLDATABASE_H
