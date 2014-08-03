#ifndef SERVERTREEITEM_H
#define SERVERTREEITEM_H

//#include <qfxmlconfigdocument.h>
#include "qfobjectitemmodel.h"
//#include <qfstring.h>
//#include <QDom>
#include <QSqlDatabase>

#include <QWidget>
#include <QVariant>

class Database;
class MainWindow;
//=============================================================
//                     ServerTreeItem
//=============================================================
class ServerTreeItem : public QObject
{
	Q_OBJECT
public slots:
	void driverDestroyed(QObject *o);
signals:
	void progressValue(double val, const  QString &label_text = QString());
protected:
	MainWindow* mainWindow();
public:
	/// @return model that owns this object
	QFObjectItemModel* model();

	virtual QVariant icon(int col) {
		Q_UNUSED(col);
		return QVariant();
	}
	virtual QVariant text(int col) {
		Q_UNUSED(col);
		return "ServerTreeItem";
	}

	Database* database();
public:
	ServerTreeItem(QObject *parent = NULL, const QString& name = "");
	virtual ~ServerTreeItem();
};

//=============================================================
//                     Connection
//=============================================================
class Database;
class Connection : public ServerTreeItem
{
	Q_OBJECT
public:
	class Params : public QVariantMap
	{
	public:
		Params(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}
		QVariant param(const QString& name) const;
		void setParam(const QString& name, const QVariant& value);
	};
public:
	Connection(const Params& _params, QObject *parent = NULL);
	virtual ~Connection();
public:
	QString connectionNameId() const;
	//static QStringList allParamNames();
	Params params() const;
	void setParams(const Params &prms);
public:
	bool isOpen();
	void close();
	Database* open();

	virtual QVariant icon(int col);
	virtual QVariant text(int col);
protected:
	int m_connectionId;
	Params m_params;
};

//=============================================================
//                     Database
//=============================================================
class Database : public ServerTreeItem
{
	Q_OBJECT
protected:
	QSqlDatabase m_sqlConnection;

	QStringList schemas() const;
public:
	virtual QVariant icon(int col);
	virtual QVariant text(int col);
signals:
	void connectionInfo(const QString &info);
public:
	QString connectionSignature();
	QStringList databases();
	const QSqlDatabase& sqlConnection() {return m_sqlConnection;}
	bool isOpen() {return m_sqlConnection.isOpen();}
	bool open();
	void close();
public:
	Database(QObject *parent = NULL, const QString& name = "");
	virtual ~Database();
};

//=============================================================
//                     Schema
//=============================================================
class Schema : public ServerTreeItem
{
	Q_OBJECT
protected:
	bool isopen;
public:
	enum CreateScriptFlags {CreateTableSql = 1, DumpTableSql = 2, IncludeViews = 4};
public:
	virtual QVariant icon(int col);
	virtual QVariant text(int col);
	void open();
	void close();
	bool isOpen() {return isopen;}
	QString createScript(int flags = (CreateTableSql | CreateTableSql | IncludeViews));
public:
	Schema(QObject *parent = NULL, const QString& name = "")
		: ServerTreeItem(parent, name), isopen(false) {}
	virtual ~Schema() {}
};

//=============================================================
//                     Table
//=============================================================
class Table : public ServerTreeItem
{
	Q_OBJECT
public:
	//enum RelationKind {SqlTable, SqlView};
	QSql::TableType kind;
public:
	virtual QVariant icon(int col);
	virtual QVariant text(int col);
	QString schema() const;
public:
	Table(QObject *parent, const QString& name, QSql::TableType _kind)
		: ServerTreeItem(parent, name), kind(_kind) {}
	virtual ~Table();
};

#endif // SERVERTREEITEM_H
