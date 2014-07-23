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
	protected:
	//static Connection *activeConnection;
	public:
		QVariantMap params;

		QString param(const QString& name);
		void setParam(const QString& name, const QString& value);
	public:
		bool isOpen();
		void close();
		Database* open();

		virtual QVariant icon(int col);
		virtual QVariant text(int col);
	public:
		Connection(const QVariantMap& _params, QObject *parent = NULL, const QString& name = "");
		virtual ~Connection();
};

//=============================================================
//                     Database
//=============================================================
class Database : public ServerTreeItem
{
	Q_OBJECT
	protected:
		QSqlDatabase sqlConnection;

		QString getConnectionId();
	public:
		virtual QVariant icon(int col);
		virtual QVariant text(int col);
	signals:
		void connectionInfo(const QString &info);
	public:
		const QSqlDatabase& connection() {return sqlConnection;}
		bool isOpen() {return sqlConnection.isOpen();}
		void open();
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
	enum RelationKind {SqlTable, SqlView};
		RelationKind kind;
	public:
		virtual QVariant icon(int col);
		virtual QVariant text(int col);
		QString schema() const;
	public:
		Table(QObject *parent, const QString& name, RelationKind _kind)
			: ServerTreeItem(parent, name), kind(_kind) {}
		virtual ~Table();
};

#endif // SERVERTREEITEM_H
