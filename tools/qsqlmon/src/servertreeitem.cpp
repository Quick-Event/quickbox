#include "theapp.h"
#include "servertreeitem.h"
#include "mainwindow.h"

#include <qf/core/utils/crypt.h>
#include <qf/core/log.h>
#include <qf/core/utils.h>
#include <qf/core/sql/dbinfo.h>

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <QIcon>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QMessageBox>

//=============================================================
//                     ServerTreeItem
//=============================================================
ServerTreeItem::ServerTreeItem(QObject *parent, const QString& name)
	: QObject(parent)
{
	//qfDebug() <<  QF_FUNC_NAME << this << name;
	setObjectName(name);
}

ServerTreeItem::~ServerTreeItem()
{
	//qfDebug() <<  QF_FUNC_NAME << this << objectName();
}

MainWindow * ServerTreeItem::mainWindow()
{
	MainWindow *ret = qfFindParent<MainWindow*>(model());
	return ret;
}

QFObjectItemModel* ServerTreeItem::model()
{
	//qfDebug() << QF_FUNC_NAME;
	QObject *o = this;
	while(o) {
		QFObjectItemModelRoot *r = qobject_cast<QFObjectItemModelRoot*>(o);
		if(r) {
			//qfDebug() << "\tmodel:" << r->model();
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
	qfDebug() << QF_FUNC_NAME << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	qfDebug() << "\tdestroyed driver:" << o;
}

//===================================================
//                 Connection
//===================================================
Connection::~Connection()
{
	//qDebug() <<  "destructor:" << this;
}

Connection::Connection(const Params &_params, QObject *parent)
	: ServerTreeItem(parent, QString())
{
	static int max_id = 0;
	m_connectionId = ++max_id;
	setParams(_params);
	//qfInfo() << params.toString();
}

bool Connection::isOpen()
{
	return children().count() > 0;
}

void Connection::close()
{
	qfDebug() << QF_FUNC_NAME;
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
	/*
	QModelIndex ix = model()->object2index(this);
	while(model()->rowCount(ix)) {
		QObject *o = model()->take(model()->index(0, 0, ix));
		qfDebug() << o->objectName();
		delete o;
	}
	*/
	//foreach(QObject *o, children()) delete o;
}

Database* Connection::open()
{
	Database *d = NULL;
	QList<QObject*> olst;

	close();
	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	d = new Database(this, m_params.param("database").toString());
	olst << d;
	bool ok = d->open();

	if(ok) {
		QStringList sl = d->databases();
		foreach(QString s, sl) {
			if(s == m_params.param("database").toString())
				continue;
			olst << new Database(this, s);
		}
		m->append(olst, ix);
	}
	else {
		//qf::qmlwidgets::dialogs::MessageBox::showError(mainWindow(), d->sqlConnection().lastError().text());
		QF_SAFE_DELETE(d);
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
	case 0: ret = m_params.param("description"); break;
	case 1: ret = m_params.param("user").toString() + "@" + m_params.param("host").toString(); break;
	case 2: ret = m_params.param("database"); break;
	case 3: ret = m_params.param("driver"); break;
	}
	return ret;
}

QString Connection::connectionNameId() const
{
	QString ret = "connection_%1";
	return ret.arg(m_connectionId);
}

QVariant Connection::Params::param(const QString& name) const
{
	//qfInfo() << QFLog::stackTrace();
	QVariant default_value = "";
	if(name == "description")
		default_value = "New_Connection";
	else if(name == "host")
		default_value = "localhost";
	//qfInfo() << "default_value:" << default_value;
	QVariant ret = value(name, default_value);
	if(name == "password") {
		ret = theApp()->crypt().decrypt(ret.toString().toLatin1());
		//qfInfo() << "password:" << s;
	}
	return ret;
}

void Connection::Params::setParam(const QString& name, const QVariant &value)
{
	QVariant val = value;
	if(name == "password") {
		//qfDebug() << "password:" << s;
		val = theApp()->crypt().encrypt(value.toString(), 32);
	}
	(*this)[name] = val;
}
/*
QStringList Connection::allParamNames()
{
	static const QStringList all_keys;
	if(all_keys.isEmpty())
		all_keys << "id"
				 << "description"
				 << "host"
				 << "port"
				 << "user"
				 << "password"
				 << "database"
				 << "driver"
				 << "textcodec"
				 << "mysqlSetNames"
				 << "sqlite_pragma_short_column_names"
				 << "sqlite_pragma_full_column_names";
	return all_keys;
}
*/
Connection::Params Connection::params() const
{
	return m_params;
}

void Connection::setParams(const Connection::Params &prms)
{
	close();
	m_params = prms;
	setObjectName(m_params.param("description").toString());
}
//===================================================


//===================================================
//                 Database
//===================================================
Database::Database(QObject *parent, const QString& name)
	: ServerTreeItem(parent, name)
{
	QObject *o = qfFindParent<MainWindow*>(this);
	if(o)
		connect(this, SIGNAL(connectionInfo(const QString&)), o, SLOT(appendInfo(const QString&)));
}

Database::~Database()
{
	//qfDebug() << QF_FUNC_NAME << "############";
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

QString Database::connectionSignature()
{
	QString s;
	Connection *c = qobject_cast<Connection*>(parent());
	if(c) {
		s = objectName() + "[" + c->params().param("driver").toString() + "]"
				+ c->params().param("user").toString() + "@" + c->params().param("host").toString()
				+ ":" + c->params().param("port").toString();
	}
	return s;
}

QStringList Database::databases()
{
	qfLogFuncFrame();
	QStringList sl;
	QSqlDriver *driver = sqlConnection().driver();
	QF_ASSERT(driver != nullptr,
			  "driver is NULL",
			  return QStringList());
	QSqlQuery q(driver->createResult());
	q.setForwardOnly(true);

	if(sqlConnection().driverName().endsWith("PSQL")) {
		q.exec(QLatin1String("SELECT datname FROM pg_database "));
		while(q.next()) {
			QString s = q.value(0).toString();
			//qfDebug() << "\tfound:" << s;
			if(s.startsWith("template"))
				continue;
			sl.append(s);
		}
	}
	else {
		sl.append(sqlConnection().databaseName());
	}
	qfDebug() << "\tloaded from server:" << sl.join(", ");
	return sl;
}

namespace {
int defaultPort(const QString &driver_name)
{
	if(driver_name.endsWith("PSQL"))
		return 5432;
	else if(driver_name.endsWith("MYSQL"))
		return 3306;
	else if(driver_name.endsWith("IBASE"))
		return 3050;
	return 0;
}
}

QStringList Database::schemas() const
{
	qfLogFuncFrame();
	QStringList ret;
	QString driver_name = m_sqlConnection.driverName();
	if(driver_name.endsWith("PSQL")) {
		QSqlQuery q(m_sqlConnection);
		q.setForwardOnly(true);
		q.exec("SELECT n.nspname "
			   " FROM pg_catalog.pg_namespace  AS n"
			   " WHERE   (n.nspname NOT LIKE 'pg\\_temp\\_%' OR"
			   " n.nspname = (pg_catalog.current_schemas(true))[1])"
			   " ORDER BY 1");
		//QSqlRecord r = q.record();
		while(q.next()) {
			QString s = q.value(0).toString();
			//qfLogFuncFrame() << "loading schema" << s;
			ret.append(s);
		}
	}
	else if(driver_name.endsWith("SQLITE")) {
		QSqlQuery q(m_sqlConnection);
		q.setForwardOnly(true);
		q.exec(QLatin1String("PRAGMA database_list"));
		//QSqlRecord r = q.record();
		while(q.next()) {
			QString s = q.value("name").toString();
			ret.append(s);
		}
	}
	else if(driver_name.endsWith("MYSQL")) {
		QSqlQuery q(m_sqlConnection);
		q.setForwardOnly(true);
		q.exec(QLatin1String("SHOW DATABASES;"));
		//QSqlRecord r = q.record();
		while(q.next()) {
			QString s = q.value(0).toString();
			ret.append(s);
		}
	}
	else if(driver_name.endsWith("IBASE")) {
		ret << "main";
	}
	qfDebug() << "\tloaded from server:" << ret.join(", ");
	return ret;
}

bool Database::open()
{
	qfLogFuncFrame();
	Connection *c = qobject_cast<Connection*>(parent());
	QF_ASSERT(c!=nullptr,
			  "Parent is not a kind of Connection",
			  return false);
	close();
	QString connection_id = c->connectionNameId();
	m_sqlConnection = QSqlDatabase::database(connection_id, false);
	QString driver_type = c->params().param("driver").toString();
	if(!m_sqlConnection.isValid())
		m_sqlConnection = QSqlDatabase::addDatabase(driver_type, connection_id);
	else if(m_sqlConnection.driverName() != driver_type) {
		m_sqlConnection = QSqlDatabase::addDatabase(driver_type, connection_id);
	}
	QF_ASSERT(m_sqlConnection.isValid(),
			  QString("Cannot add database for '%1' named '%2'").arg(c->params().param("driver").toString()).arg(connection_id),
			  return false);
	//QObject::connect(sqlConnection.driver(), SIGNAL(destroyed(QObject*)), this->parent(), SLOT(driverDestroyed(QObject*)));
	m_sqlConnection.setHostName(c->params().param("host").toString());
	m_sqlConnection.setPort(c->params().param("port").toInt());
	m_sqlConnection.setUserName(c->params().param("user").toString());
	m_sqlConnection.setPassword(c->params().param("password").toString());
	//qDebug() << sqlConnection.password();
	m_sqlConnection.setDatabaseName(objectName());
	//qfDebug() << "\t" << m_sqlConnection.info();
	QStringList opts;
	//opts["QF_CODEC_NAME"] = theApp()->config()->value("/i18n/dbtextcodec", "UTF-8").toString();
	if(driver_type.endsWith("MYSQL")) {
		QString codec_name = c->params().param("textcodec").toString();
		if(!codec_name.isEmpty())
			opts << "QF_CODEC_NAME=" + codec_name;
		QString set_names = c->params().param("mysqlSetNames").toString();
		if(!set_names.isEmpty() && !set_names.startsWith("<"))
			opts << "QF_MYSQL_SET_NAMES=" << set_names; /// konfigurak dava <no change> pro nic
	}
	//m_sqlConnection.setJournal(theApp()->sqlJournal());
	//m_sqlConnection.open(opts);
	if(m_sqlConnection.port() == 0)
		m_sqlConnection.setPort(defaultPort(m_sqlConnection.driverName()));
	if(!opts.isEmpty()) {
		m_sqlConnection.setConnectOptions(opts.join(";"));
	}
	//qfInfo() << "password:" << password();
	if(!m_sqlConnection.open()) {
		m_sqlConnection.setConnectOptions();
		qf::qmlwidgets::dialogs::MessageBox::showError(mainWindow(),
													   tr("Error opening database %1").arg(connectionSignature())
													   + "\n\n"
													   + m_sqlConnection.lastError().text());
		return false;
	}
	if(driver_type.endsWith("SQLITE")) {
		{
			QString s = c->params().param("sqlite_pragma_full_column_names").toString();
			if(s != "1")
				s = "0";
			QSqlQuery q(m_sqlConnection);
			//qfDebug() << "\texecuting:" << "PRAGMA full_column_names = 1";
			q.exec("PRAGMA full_column_names = " + s);
		}
		{
			QString s = c->params().param("sqlite_pragma_short_column_names").toString();
			if(s != "1") s = "0";
			QSqlQuery q(m_sqlConnection);
			q.exec("PRAGMA short_column_names = " + s);
		}
	}
	qfDebug() << "\tdriver:" << m_sqlConnection.driver();
	//qfDebug() << "\tcodec:" << opts["QF_CODEC_NAME"];
	qfDebug() << "\tis open:" << m_sqlConnection.isOpen();
	QString s = m_sqlConnection.driver()->property("connectionInfo").toString();
	qfDebug() << "\tinfo:" << s;
	emit connectionInfo(s);

	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	QList<QObject*> olst;
	if(m_sqlConnection.driverName().endsWith("IBASE")) {
		/// tables
		QStringList sl;
		sl = m_sqlConnection.tables(QSql::Tables);
		qSort(sl);
		foreach(QString s, sl)
			olst << new Table(this, s, QSql::Tables);
		sl = m_sqlConnection.tables(QSql::Views);
		qSort(sl);
		foreach(QString s, sl)
			olst << new Table(this, s, QSql::Views);
	}
	else {
		foreach(QString s, schemas()) {
			Schema *sch = new Schema(NULL, s);
			olst << sch;
			connect(sch, SIGNAL(progressValue(double, const QString&)), mainWindow(), SLOT(setProgressValue(double, const QString&)));
		}
	}
	m->append(olst, ix);
	
	return true;
}

void Database::close()
{
	qfLogFuncFrame() << this << "Database::close(): " << connectionSignature();
	m_sqlConnection.close();
	m_sqlConnection = QSqlDatabase(); // zrus referenci na databasi

	// vymaz deti
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
}

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

QString Schema::createScript(int flags)
{
	qfDebug() << QF_FUNC_NAME;

	QString ret;
	if(!isOpen())
		open();
	
	// find parent database
	Database *d = database();
	if(!d) return "cann't find database";
	QSqlDatabase c = d->sqlConnection();
	if(c.driverName().endsWith("SQLITE")) {
		QSqlQuery q(c);
		q.exec("SELECT sql FROM main.sqlite_master WHERE NAME NOT LIKE 'sqlite_%'");
		QStringList sl;
		while(q.next()) {
			sl << q.value(0).toString() + ';';
		}
		ret = sl.join("\n");
	}
	else {
		Q_UNUSED(flags);
#if 0
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
		qfDebug() << "\t tables count:" << sl.count();
		double cnt = sl.count();
		int no = 1;
		foreach(QString s, sl_tables) {
			QFSqlTableInfo ti = di.table(s);
			QString tbl_name = ti.fullName();
			emit progressValue(no++/cnt, ti.tableName());
			qfDebug() << "\t table name:" << tbl_name;
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
			qfDebug() << "\t table name:" << tbl_name;
			if(flags & CreateTableSql) {
				ret += c.createTableSqlCommand(tbl_name);
				ret += "\n\n";
			}
		}
#endif
	}
	emit progressValue(-1);
	return ret;
}

void Schema::open()
{
	qfLogFuncFrame();
	if(isOpen())
		return;
	/// find parent database
	Database *d = database();
	if(!d) return;
	/*
	QSqlDatabase c = d->sqlConnection();
	QFSqlCatalog &cat = c.catalog();
	if(c.driverName().endsWith("MYSQL")) {
		cat.setCurrentSchema(objectName());
	}
	*/
	QString schema_name = objectName();
	QList<QObject*> olst;
	/// tables
	qf::core::sql::DbInfo dbi(d->sqlConnection());
	QStringList sl = dbi.tables(schema_name, QSql::Tables);
	qSort(sl);
	foreach(QString s, sl)
		olst << new Table(nullptr, s, QSql::Tables);
	sl = dbi.tables(schema_name, QSql::Views);
	qSort(sl);
	foreach(QString s, sl)
		olst << new Table(nullptr, s, QSql::Views);
	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	m->append(olst, ix);
#if 0
	QFSqlDbInfo di = cat.database(objectName());
	QStringList sl_tables = di.tables(QFSql::AllRelations & ~QFSql::ViewRelation);
	QStringList sl_views = di.tables(QFSql::ViewRelation);
	qSort(sl_tables);
	QFObjectItemModel *m = model();
	QModelIndex ix = m->object2index(this);
	QList<QObject*> olst;
	foreach(QString s, sl_tables) {
		qfDebug() << "\t adding table" << s;
		olst << new Table(NULL, s, QSql::Tables);
	}
	qSort(sl_views);
	foreach(QString s, sl_views) {
		qfDebug() << "\t adding view" << s;
		olst << new Table(NULL, s, QSql::Views);
	}
	m->append(olst, ix);
	/*
	if(c.driverType() == QSqlDatabase::PSQL) {
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
		QSqlQuery q(c);
		q.exec(s.arg(objectName()));
		foreach(QSqlQuery::Row row, q.rows()) {
			QString kind = row.value(2).toString();
			if(kind == "view") new Table(this, row.value(1).toString(), Table::KindView);
			if(kind == "table") new Table(this, row.value(1).toString(), Table::KindTable);
		}
	}
	*/
#endif
	isopen = true;
}

void Schema::close()
{
	//qfDebug() << QF_FUNC_NAME;
	QModelIndex ix = model()->object2index(this);
	model()->deleteChildren(ix);
	isopen = false;
	Database *d = database();
	if(!d) return;
}

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
		if(kind == QSql::Tables)
			return qVariantFromValue(QIcon(":/images/table.png"));
		else if(kind == QSql::Views)
			return qVariantFromValue(QIcon(":/images/view.png"));
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


