#include "theapp.h"
#include "servertreeitem.h"
#include "mainwindow.h"

//#include <qf.h>
#include <qfpixmapcache.h>
#include <qfexception.h>
#include <qfdlgexception.h>
#include <qfstring.h>
#include <qfsqlquery.h>
#include <qfsqlcatalog.h>
#include <qfcrypt.h>

#include <QIcon>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>

//#define QF_NO_TRASH_OUTPUT
#include <qflogcust.h>

//=============================================================
//                     ServerTreeItem
//=============================================================
ServerTreeItem::ServerTreeItem(QObject *parent, const QString& name)
	: QObject(parent)
{
	//qfTrash() <<  QF_FUNC_NAME << this << name;
	setObjectName(name);
}

ServerTreeItem::~ServerTreeItem()
{
	//qfTrash() <<  QF_FUNC_NAME << this << objectName();
}

MainWindow * ServerTreeItem::mainWindow() throw( QFException )
{
	MainWindow *ret = qfFindParent<MainWindow*>(model());
	return ret;
}

QFObjectItemModel* ServerTreeItem::model()
{
	//qfTrash() << QF_FUNC_NAME;
	QObject *o = this;
	while(o) {
		QFObjectItemModelRoot *r = qobject_cast<QFObjectItemModelRoot*>(o);
		if(r) {
			//qfTrash() << "\tmodel:" << r->model();
			//r->model()->dumpObjectInfo();
			return r->model();
		}
		o = o->parent();
	}
	Q_ASSERT(o != NULL);
	return NULL;
}

Database* ServerTreeItem::database()
{
	QObject *o = this;
	Database *d = NULL;
	while(o) {
		d = qobject_cast<Database*>(o);
		if(d) break;
		o = o->parent();
	}
	return d;
}

void ServerTreeItem::driverDestroyed(QObject *o)
{
	qfTrash() << QF_FUNC_NAME << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	qfTrash() << "\tdestroyed driver:" << o;
}

//===================================================
//                 Connection
//===================================================
Connection::~Connection()
{
	//qDebug() <<  "destructor:" << this;
}

Connection::Connection(const QVariantMap &_params, QObject *parent, const QString& name)
		: ServerTreeItem(parent, name), params(_params)
{
	setObjectName(param("description"));
	//qfInfo() << params.toString();
}

bool Connection::isOpen()
{
    return children().count() > 0;
}

void Connection::close()
{
	qfTrash() << QF_FUNC_NAME;
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
	/*
	QModelIndex ix = model()->object2index(this);
	while(model()->rowCount(ix)) {
		QObject *o = model()->take(model()->index(0, 0, ix));
		qfTrash() << o->objectName();
		delete o;
	}
	*/
    //foreach(QObject *o, children()) delete o;
}

Database* Connection::open() throw(QFException)
{
	Database *d = NULL;
	QList<QObject*> olst;
	try {
		close();
		QFObjectItemModel *m = model();
		QModelIndex ix = m->object2index(this);
		d = new Database(this, param("database"));
		olst << d;
		d->open();
		QStringList sl = d->connection().databases();
		foreach(QString s, sl) {
			if(s == param("database")) continue;
			olst << new Database(this, s);
		}
		m->append(olst, ix);
	}
	catch(QFException &e) {
		QFDlgException dlg; dlg.exec(e);
		qDeleteAll(olst);
		d = NULL;
	}
	return d;
}


QVariant Connection::icon(int col)
{
	static QIcon ico;
	static bool first_scan = true;
	if(first_scan) {
		first_scan = false;
		ico.addFile(":/images/server_on.png", QSize(), QIcon::Normal, QIcon::On);
		ico.addFile(":/images/server_off.png", QSize(), QIcon::Normal, QIcon::Off);
	}
	if(col == 0) return qVariantFromValue(ico);
	return QVariant();
}

QVariant Connection::text(int col)
{
    QVariant ret;
    switch(col) {
		case 0: ret = param("description"); break;
		case 1: ret = param("user") + "@" + param("host"); break;
		case 2: ret = param("database"); break;
		case 3: ret = param("driver"); break;
    }
	return ret;
}

QString Connection::param(const QString& name)
{
	//qfInfo() << QFLog::stackTrace();
	QString default_value = "";
	if(name == "description") default_value = "New_Connection";
	else if(name == "host") default_value = "localhost";
	//qfInfo() << "default_value:" << default_value;
	QFString s = params.cd(name, !Qf::ThrowExc).value(default_value).toString();
	if(name == "password") {
		s = QFCrypt().decrypt(s.toLatin1());
		//qfInfo() << "password:" << s;
	}
	return s;
}

void Connection::setParam(const QString& name, const QString& value)
{
	Q_ASSERT(params.isElement());
	QFString s = value;
	if(name == "password") {
		//qfTrash() << "password:" << s;
		s = QString::fromLatin1(QFCrypt().crypt(s));
	}
	params.setValue(name, s);
}
//===================================================


//===================================================
//                 Database
//===================================================
Database::Database(QObject *parent, const QString& name)
	: ServerTreeItem(parent, name)
{
	QObject *o = Qf::findParentOfType(this, "MainWindow");
	if(o) connect(this, SIGNAL(connectionInfo(const QString&)), o, SLOT(appendInfo(const QString&)));
}

Database::~Database()
{
	//qfTrash() << QF_FUNC_NAME << "############";
	//close();
	//qDebug() <<  "destructor:" << this;
}

QVariant Database::icon(int col)
{
	static QIcon ico_open;
	static QIcon ico_closed;
	static bool first_scan = true;
	if(first_scan) {
		first_scan = false;
		ico_open.addFile(":/images/database_on.png", QSize(), QIcon::Normal);//, QIcon::On);
		ico_closed.addFile(":/images/database_off.png", QSize(), QIcon::Normal);//, QIcon::Off);
	}
	if(col == 0) {
		if(isOpen()) return qVariantFromValue(ico_open);
		return qVariantFromValue(ico_closed);
	}
	return QVariant();
}

QVariant Database::text(int col)
{
	QVariant ret;
	if(col == 0) ret = objectName();
	return ret;
}

QString Database::getConnectionId()
{
	QString s;
	Connection *c = qobject_cast<Connection*>(parent());
	if(c) {
		s = objectName() + "[" + c->param("driver") + "]" + c->param("user") + "@" + c->param("host") + ":" + c->param("port");
	}
	return s;
}

void Database::open() throw(QFException)
{
	qfTrash() << QF_FUNC_NAME;
	Connection *c = qobject_cast<Connection*>(parent());
	if(!c) return;
	/*
	sqlConnection = QFSqlConnection(QSqlDatabase::database(getConnectionId()));
	if(!sqlConnection.isValid()) {
		sqlConnection = QFSqlConnection(QSqlDatabase::addDatabase(c->param("driver"), getConnectionId()));
	}
	if(!sqlConnection.isValid()) {
		QString s = tr("Error creating SQL connection %1").arg(getConnectionId())
			+ QFEOLN + QFEOLN
			+ sqlConnection.lastError().text();
		throw QFException(s);
	}
	*/
	sqlConnection.close();
	sqlConnection = QFSqlConnection(c->param("driver"));
	//QObject::connect(sqlConnection.driver(), SIGNAL(destroyed(QObject*)), this->parent(), SLOT(driverDestroyed(QObject*)));
	sqlConnection.setHostName(c->param("host"));
	sqlConnection.setPort(c->param("port").toInt());
	sqlConnection.setUserName(c->param("user"));
	sqlConnection.setPassword(c->param("password"));
	//qDebug() << sqlConnection.password();
	sqlConnection.setDatabaseName(objectName());
	qfTrash() << "\t" << sqlConnection.info();
	QFSqlConnection::ConnectionOptions opts;
	//opts["QF_CODEC_NAME"] = theApp()->config()->value("/i18n/dbtextcodec", "UTF-8").toString();
	QString codec_name = c->param("textcodec");
	if(!codec_name.isEmpty()) opts["QF_CODEC_NAME"] = codec_name;
	QString set_names = c->param("mysqlSetNames");
	if(!set_names.isEmpty() && !set_names.startsWith("<")) opts["QF_MYSQL_SET_NAMES"] = set_names; /// konfigurak dava <no change> pro nic
	sqlConnection.setJournal(theApp()->sqlJournal());
	sqlConnection.open(opts);
	/*
	if(c->param("driver").endsWith("MYSQL")) {
		QString set_names = c->param("mysqlSetNames");
		if(!set_names.isEmpty() && set_names[0] != '<') {
			QFSqlQuery q(sqlConnection);
			q.exec("SET NAMES "SARG(set_names));
		}
	}
	else
		*/
	if(c->param("driver").endsWith("SQLITE")) {
		{
			QString s = c->param("sqlite_pragma_full_column_names");
			if(s != "1") s = "0";
			QFSqlQuery q(sqlConnection);
			//qfTrash() << "\texecuting:" << "PRAGMA full_column_names = 1";
			q.exec("PRAGMA full_column_names = " + s);
		}
		{
			QString s = c->param("sqlite_pragma_short_column_names");
			if(s != "1") s = "0";
			QFSqlQuery q(sqlConnection);
			q.exec("PRAGMA short_column_names = " + s);
		}
	}
	qfTrash() << "\tdriver:" << sqlConnection.driver();
	qfTrash() << "\tcodec:" << opts["QF_CODEC_NAME"];
	qfTrash() << "\tis open:" << sqlConnection.isOpen();
	QString s = sqlConnection.driver()->property("connectionInfo").toString();
	qfTrash() << "\tinfo:" << s;
	emit connectionInfo(s);

	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	QList<QObject*> olst;
	if(sqlConnection.driverName().endsWith("IBASE")) {
		/// tables
		QStringList sl;
		sl = sqlConnection.tables("cokoli", QSql::Tables);
		qSort(sl);
		foreach(QString s, sl) olst << new Table(this, s, QFSql::TableRelation);
		sl = sqlConnection.tables("cokoli", QSql::Views);
		qSort(sl);
		foreach(QString s, sl) olst << new Table(this, s, QFSql::ViewRelation);
	}
	else {
		foreach(QString s, sqlConnection.schemas()) {
			Schema *sch = new Schema(NULL, s);
			olst << sch;
			connect(sch, SIGNAL(progressValue(double, const QString&)), mainWindow(), SLOT(setProgressValue(double, const QString&)));
		}
	}
	m->append(olst, ix);
	
	//QFObjectItemModel *m = model();
	//int n = sl.count();
	//if(n > 0)
	//	m->emitRowsInserted(m->object2index(this), 0, n-1);
}

void Database::close()
{
	qfTrash() << this << "Database::close(): " << sqlConnection.info();
	sqlConnection.close();
	sqlConnection = QSqlDatabase(); // zrus referenci na databasi

	// vymaz deti
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
	//QSqlDatabase::removeDatabase(getConnectionId());
	// delete tables
	/*
	QObjectList chld_lst = children();
	while(!chld_lst.isEmpty())
        delete chld_lst.takeFirst();
	*/
	//QFObjectItemModel *m = model();
	/*
	int n = children().count();
	if(n > 0) {
		//m->emitRowsAboutToBeRemoved(m->object2index(this), 0, n-1);
		// objekt se v destruktoru sam vyjme ze seznamu
		foreach(QObject *o, children()) {
			delete o; // nevim, jestli se to nebude sekat, nemelo by
			//o->deleteLater(); // tohle je tutovka
		}
	}
	*/
}

//===================================================

//=============================================================
//                     Schema
//=============================================================
QVariant Schema::icon(int col)
{
	static QIcon ico;
	static bool first_scan = true;
	if(first_scan) {
		first_scan = false;
		ico.addFile(":/images/schema.png");
	}
	if(col == 0) return qVariantFromValue(ico);
	return QVariant();
}

QVariant Schema::text(int col)
{
	QVariant ret;
	if(col == 0) ret = objectName();
	return ret;
}

QString Schema::createScript(int flags) throw(QFException)
{
	qfTrash() << QF_FUNC_NAME;

	QString ret;
	if(!isOpen()) open();
	
	// find parent database
	Database *d = database();
	if(!d) return "cann't find database";
	QFSqlConnection c = d->connection();
	if(c.driverName().endsWith("SQLITE")) {
		QFSqlQuery q(c);
		q.exec("SELECT sql FROM main.sqlite_master WHERE NAME NOT LIKE 'sqlite_%'");
		QStringList sl;
		while(q.next()) {
			sl << q.value(0).toString() + ';';
		}
		ret = sl.join("\n");
	}
	else {
		QFSqlCatalog &cat = c.catalog();
		if(c.driverName().endsWith("MYSQL")) {
			cat.setCurrentSchema(objectName());
		}
		QFSqlDbInfo di = cat.database(objectName());
		QStringList sl = di.tables();
		QStringList sl_tables;
		QStringList sl_views;
		foreach(QString s, sl) {
			QFSqlTableInfo ti = di.table(s);
			if(ti.relationKind() == QFSql::ViewRelation) sl_views << s;
			else sl_tables << s;
		}
		foreach(QString s, sl_tables) {
			QFSqlTableInfo ti = di.table(s);
			new Table(this, s, ti.relationKind());
		}
		foreach(QString s, sl_views) {
			new Table(this, s, QFSql::ViewRelation);
		}
		qfTrash() << "\t tables count:" << sl.count();
		double cnt = sl.count();
		int no = 1;
		foreach(QString s, sl_tables) {
			QFSqlTableInfo ti = di.table(s);
			QString tbl_name = ti.fullName();
			emit progressValue(no++/cnt, ti.tableName());
			qfTrash() << "\t table name:" << tbl_name;
			if(flags & CreateTableSql) {
				ret += c.createTableSqlCommand(tbl_name);
				ret += "\n\n";
			}
			if(flags & DumpTableSql) {
				ret += c.dumpTableSqlCommand(tbl_name);
				ret += "\n\n";
			}
		}
		if(flags & IncludeViews) foreach(QString s, sl_views) {
			QFSqlTableInfo ti = di.table(s);
			QString tbl_name = ti.fullName();
			emit progressValue(no++/cnt, ti.tableName());
			qfTrash() << "\t table name:" << tbl_name;
			if(flags & CreateTableSql) {
				ret += c.createTableSqlCommand(tbl_name);
				ret += "\n\n";
			}
		}
	}
	emit progressValue(-1);
	return ret;
}

void Schema::open() throw(QFException)
{
	qfLogFuncFrame();
	if(isOpen()) return;
	/// find parent database
	Database *d = database();
	if(!d) return;
	QFSqlConnection c = d->connection();
	QFSqlCatalog &cat = c.catalog();
	if(c.driverName().endsWith("MYSQL")) {
		cat.setCurrentSchema(objectName());
	}
	QFSqlDbInfo di = cat.database(objectName());
	QStringList sl_tables = di.tables(QFSql::AllRelations & ~QFSql::ViewRelation);
	QStringList sl_views = di.tables(QFSql::ViewRelation);
	qSort(sl_tables);
	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	QList<QObject*> olst;
	foreach(QString s, sl_tables) {
		qfTrash() << "\t adding table" << s;
		olst << new Table(NULL, s, QFSql::TableRelation);
	}
	qSort(sl_views);
	foreach(QString s, sl_views) {
		qfTrash() << "\t adding view" << s;
		olst << new Table(NULL, s, QFSql::ViewRelation);
	}
	m->append(olst, ix);
	/*
	if(c.driverType() == QFSqlConnection::PSQL) {
		QString s = "SELECT n.nspname,	c.relname,"
							"	CASE c.relkind WHEN 'r' THEN 'table' WHEN 'v' THEN 'view' WHEN 'i' THEN 'index' WHEN 'S' THEN 'sequence' WHEN 's' THEN 'special' END AS type,"
							"	u.usename,"
							"  pg_catalog.obj_description(c.oid, 'pg_class') AS descr"
							" FROM pg_catalog.pg_class c"
							" LEFT JOIN pg_catalog.pg_user u ON u.usesysid = c.relowner"
							" LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace"
							" WHERE c.relkind IN ('r','v')"
							"  AND n.nspname = '%1'"
							" ORDER BY 1,2;";
		QFSqlQuery q(c);
		q.exec(s.arg(objectName()));
		foreach(QFSqlQuery::Row row, q.rows()) {
			QString kind = row.value(2).toString();
			if(kind == "view") new Table(this, row.value(1).toString(), Table::KindView);
			if(kind == "table") new Table(this, row.value(1).toString(), Table::KindTable);
		}
	}
	*/

	isopen = true;
}

void Schema::close()
{
	//qfTrash() << QF_FUNC_NAME;
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
	isopen = false;
	Database *d = database();
	if(!d) return;
	QFSqlConnection c = d->connection();
	QFSqlCatalog &cat = c.catalog();
	cat.forgetDatabase(objectName());
}
//===================================================

//===================================================
//                 Table
//===================================================
Table::~Table()
{
	//qDebug() <<  "destructor:" << this;
}

QVariant Table::icon(int col)
{
	if(col == 0) {
		if(kind == QFSql::TableRelation) return qVariantFromValue(QFPixmapCache::icon("icon.table", ":/images/table.png"));
		else if(kind == QFSql::ViewRelation) return qVariantFromValue(QFPixmapCache::icon("icon.view", ":/images/view.png"));
		else if(kind == QFSql::SystemTableRelation) return qVariantFromValue(QFPixmapCache::icon("icon.systemTable", ":/images/systemtable.png"));
	}
	return QVariant();
}

QVariant Table::text(int col)
{
	QVariant ret;
	if(col == 0) ret = objectName();
	return ret;
}

QString Table::schema() const
{
	const QObject *o = this;
	const Schema *d = NULL;
	while(o) {
		d = qobject_cast<const Schema*>(o);
		if(d) return d->objectName();
		o = o->parent();
	}
	return "";
}
//===================================================


