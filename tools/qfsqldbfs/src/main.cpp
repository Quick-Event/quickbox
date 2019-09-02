#include "thisfuse.h"
#include "theapp.h"
#include "dbfsfuseops.h"
#include "fusethread.h"

#include <qf/core/sql/dbfsdriver.h>
#include <qf/core/sql/dbfsattrs.h>
#include <qf/core/utils/table.h>
#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QSqlDatabase>
#include <QSqlError>

#include <iostream>
#include <signal.h>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

#define USE_QT_EVENT_LOOP

namespace qfs = qf::core::sql;

#ifdef USE_QT_EVENT_LOOP
static FuseThread *s_fuseThread = nullptr;

static void exitHandler(int)
{
	if (s_fuseThread != nullptr)
	{
		qfInfo() << "SIG INT";
		qfInfo() << "unmouting FUSE FS";
		s_fuseThread->unmount();
	}
}


static void set_signal_handlers()
{
	struct sigaction sa;

	sa.sa_handler = exitHandler;
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = 0;

	if (sigaction(SIGHUP, &sa, nullptr) == -1
			|| sigaction(SIGINT, &sa, nullptr) == -1
			|| sigaction(SIGQUIT, &sa, nullptr) == -1
			|| sigaction(SIGTERM, &sa, nullptr) == -1)
	{
		qfError()<<"Cannot set exit signal handlers.";
		exit(1);
	}

	sa.sa_handler = SIG_IGN;

	if (sigaction(SIGPIPE, &sa, nullptr) == -1)
	{
		qfError()<<"Cannot set ignored signals.";
		exit(1);
	}
}
#endif

int main(int argc, char *argv[])
{
	if(argc <= 2) {
		QString arg = "-h";
		if(argc > 1)
			arg = argv[1];
		if(arg == QStringLiteral("-h") || arg == QStringLiteral("--help")) {
			std::cout << argv[0] << " FUSE_options --dbfs DBFS_options" << std::endl;
			std::cout << "FUSE_options" << std::endl;
			std::cout << "\tuse -h --dbfs switch to print FUSE options" << std::endl;
			std::cout << "DBFS_options" << std::endl;
			std::cout << "\t--dbfs\t" << "DBFS options separator, all options prior this will be ignored by DBFS" << std::endl;
			std::cout << "\t--host <host>\t" << "Database host" << std::endl;
			std::cout << "\t--port <port>\t" << "Database port" << std::endl;
			std::cout << "\t -u" << std::endl;
			std::cout << "\t--user <user>\t" << "Database user" << std::endl;
			std::cout << "\t -p" << std::endl;
			std::cout << "\t--password [<password>]\t" << "Database user password, use -p without password for interactive input." << std::endl;
			std::cout << "\t--database <database>\t" << "Database name" << std::endl;
			std::cout << "\t--db-schema\t" << "Database schema name" << std::endl;
			std::cout << "\t--table-name\t" << "DBFS table name, default is 'dbfs'" << std::endl;
			std::cout << "\t--create\t" << "Create DBFS tables" << std::endl;
			exit(0);
		}
	}

	int dbfs_switch_index;
	for(dbfs_switch_index = 1; dbfs_switch_index < argc; dbfs_switch_index++)
		if(argv[dbfs_switch_index] == QLatin1String("--dbfs")) {
			break;
		}

	QScopedPointer<qf::core::LogDevice> file_log_device(qf::core::FileLogDevice::install());
	qf::core::LogDevice::setGlobalTresholds(argc - dbfs_switch_index, argv + dbfs_switch_index);
	//file_log_device->setPrettyDomain(true);

	QString o_database;
	QString o_user;
	QString o_password;
	QString o_host;
	int o_port = 0;
	QString o_db_schema;
	QString o_table_name;
	bool o_create_db = false;
	bool o_ask_passwd = false;

	for(int i=dbfs_switch_index + 1; i<argc; i++) {
		QString arg = argv[i];
		if(arg == QStringLiteral("--host")) {
			if(i<argc-1) {
				i++;
				o_host = argv[i];
			}
		}
		else if(arg == QStringLiteral("--port")) {
			if(i<argc-1) {
				i++;
				o_port = QString(argv[i]).toInt();
			}
		}
		else if(arg == QStringLiteral("-u") || arg == QStringLiteral("--user")) {
			if(i<argc-1) {
				i++;
				o_user = argv[i];
			}
		}
		else if(arg == QStringLiteral("-p") || arg == QStringLiteral("--password")) {
			if(i<argc-1) {
				QString p = argv[i+1];
				if(p.startsWith('-')) {
					o_ask_passwd = true;
				}
				else {
					o_password = p;
					i++;
				}
			}
			else {
				o_ask_passwd = true;
			}
		}
		else if(arg == QStringLiteral("--database")) {
			if(i<argc-1) {
				i++;
				o_database = argv[i];
			}
		}
		else if(arg == QStringLiteral("--db-schema")) {
			if(i<argc-1) {
				i++;
				o_db_schema = argv[i];
			}
		}
		else if(arg == QStringLiteral("--table-name")) {
			if(i<argc-1) {
				i++;
				o_table_name = argv[i];
			}
		}
		else if(arg == QStringLiteral("--create")) {
			o_create_db = true;
		}
	}

	if(o_ask_passwd) {
		char pwd[256];
		std::cout << "Please, enter your password: ";
		termios oldt;
		tcgetattr(STDIN_FILENO, &oldt);
		termios newt = oldt;
		newt.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		std::cin.getline(pwd, 256);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		o_password = QString::fromUtf8(pwd);
	}

	if(o_database.isEmpty()) {
		qfError() << "Empty database name.";
		exit(1);
	}

	qfs::DbFsDriver *dbfs_drv = nullptr;
	qf::core::sql::Connection db_connection;
	if(dbfs_switch_index < (argc - 1)) {
		db_connection = QSqlDatabase::addDatabase("QPSQL");
		db_connection.setHostName(o_host);
		if(o_port > 0)
			db_connection.setPort(o_port);
		db_connection.setDatabaseName(o_database);
		db_connection.setUserName(o_user);
		db_connection.setPassword(o_password);
		//qfInfo() << o_host << o_port << o_user << o_database;
		bool ok = db_connection.open();
		if(!ok) {
			qfError() << db_connection.lastError().text();
			exit(1);
		}
		if(!o_db_schema.isEmpty()) {
			if(!db_connection.setCurrentSchema(o_db_schema)) {
				qfError() << "Error setting db schema to:" << o_db_schema;
				exit(1);
			}
		}
		dbfs_drv = new qfs::DbFsDriver();
		dbfs_drv->setConnectionName(db_connection.connectionName());

		if(!o_table_name.isEmpty()) {
			dbfs_drv->setTableName(o_table_name);
		}
		if(o_create_db) {
			if(!dbfs_drv->createDbFs()) {
				qfError() << "Error creating dbfs table" << dbfs_drv->tableName();
			}
			exit(1);
		}

		qfsqldbfs_setdriver(dbfs_drv);
	}

	int fuse_argc = dbfs_switch_index;

	/// FUSE variables
	struct fuse_args fuse_arguments = FUSE_ARGS_INIT(fuse_argc, argv);
	struct fuse_chan *fuse_channel = nullptr;
	struct fuse *fuse_handle = nullptr;
	char *mount_point = nullptr;

	if (fuse_parse_cmdline(&fuse_arguments, &mount_point, nullptr, nullptr) == -1) {
		qfError() << "fuse_parse_cmdline() - Error parsing fuse command line arguments!";
		exit(1);
	}

	/// Tell FUSE where the local mountpoint is
	fuse_channel = fuse_mount(mount_point, &fuse_arguments);
	if (fuse_channel == nullptr){
		qfError()<<"fuse_mount() failed";
		exit(1);
	}

	// Tell FUSE about implementations of FS operations
	struct fuse_operations fuse_ops;
	memset(&fuse_ops, 0, sizeof(fuse_ops));
	fuse_ops.getattr = qfsqldbfs_getattr;
	fuse_ops.readdir = qfsqldbfs_readdir;
	fuse_ops.open = qfsqldbfs_open;
	fuse_ops.read = qfsqldbfs_read;
	fuse_ops.write = qfsqldbfs_write;
	fuse_ops.fsync = qfsqldbfs_fsync;
	fuse_ops.flush = qfsqldbfs_flush;
	fuse_ops.release = qfsqldbfs_release;
	fuse_ops.mknod = qfsqldbfs_mknod;
	fuse_ops.mkdir = qfsqldbfs_mkdir;
	fuse_ops.unlink = qfsqldbfs_unlink;
	fuse_ops.rmdir = qfsqldbfs_rmdir;
	fuse_ops.utime = qfsqldbfs_utime;
	fuse_ops.truncate = qfsqldbfs_truncate;
	fuse_ops.ftruncate = qfsqldbfs_ftruncate;
	fuse_ops.chmod = qfsqldbfs_chmod;
	fuse_ops.chown = qfsqldbfs_chown;
	fuse_ops.create = qfsqldbfs_create;
	fuse_ops.rename = qfsqldbfs_rename;

	fuse_handle = fuse_new(fuse_channel, &fuse_arguments, &fuse_ops, sizeof(fuse_ops), nullptr);
	if (fuse_handle == nullptr){
		qfError()<<"fuse_new() failed";
		exit(1);
	}

	if(dbfs_drv) {
#ifdef USE_QT_EVENT_LOOP
		qfInfo() << "Using Qt event loop with FUSE in separated thread";
		TheApp *app = new TheApp(argc, argv);
		s_fuseThread = new FuseThread(fuse_handle, fuse_channel, QString::fromUtf8(mount_point));
		dbfs_drv->moveToThread(s_fuseThread);
		QObject::connect(s_fuseThread, &QThread::finished, app, &TheApp::onFuseThreadFinished, Qt::QueuedConnection);
		s_fuseThread->start();

		set_signal_handlers();

		{
			/// setup SQL notify
			QSqlDatabase notify_db = QSqlDatabase::addDatabase("QPSQL", "DBFS_Notify");
			notify_db.setHostName(db_connection.hostName());
			notify_db.setPort(db_connection.port());
			notify_db.setUserName(db_connection.userName());
			notify_db.setPassword(db_connection.password());
			notify_db.setDatabaseName(db_connection.databaseName());
			bool ok = notify_db.open();
			if(!ok) {
				qfError() << "Error connect DBFS notify connection" << notify_db.lastError().text();
			}
			else {
				QSqlDriver *drv = notify_db.driver();
				//qRegisterMetaType<QSqlDriver::NotificationSource>("QSqlDriver::NotificationSource");
				QObject::connect(drv, SIGNAL(notification(QString,QSqlDriver::NotificationSource,QVariant)), dbfs_drv, SLOT(onSqlNotify(QString,QSqlDriver::NotificationSource,QVariant)), Qt::DirectConnection);
				//QObject::connect(drv, SIGNAL(notification(QString,QSqlDriver::NotificationSource,QVariant)), app, SLOT(onSqlNotify(QString,QSqlDriver::NotificationSource,QVariant)));
				drv->subscribeToNotification(qfs::DbFsDriver::CHANNEL_INVALIDATE_DBFS_DRIVER_CACHE);
				qfInfo() << drv << "subscribedToNotifications:" << drv->subscribedToNotifications().join(", ");
			}
		}

		app->exec();

		qfInfo() << "Waiting for FUSE thread to join ...";
		s_fuseThread->wait();
#else
		qfInfo() << "Using FUSE event loop";
		fuse_loop(fuse_handle);
		qfInfo() << "FUSE has quit its event loop";
#endif

		qfsqldbfs_setdriver(nullptr);
		QF_SAFE_DELETE(dbfs_drv);
#ifdef USE_QT_EVENT_LOOP
		QF_SAFE_DELETE(s_fuseThread);
		QF_SAFE_DELETE(app);
#endif
	}
	else {
		// used just to print FUSE help
		fuse_loop(fuse_handle);
	}

	qfInfo() << "bye";
	return 0;
}
