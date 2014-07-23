
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#include "theapp.h"
#include "driver/qfhttpmysql/qfhttpmysql.h"

#include <qflogcust.h>

//======================================================
//                   SqlJournal
//======================================================
void SqlJournal::log(const QString& msg)
{
	if(msg.startsWith("select ", Qt::CaseInsensitive)) return;
	if(msg.startsWith("show ", Qt::CaseInsensitive)) return;
	if(msg.startsWith("use ", Qt::CaseInsensitive)) {
		/// pokud minule use bylo stejne, neloguj ho
		if(m_log.count() && m_log.last() == msg) return;
	}
	//qfInfo() << msg;
	m_log << msg;
}

//======================================================
//                   QFHttpMySqlDriverCreator
//======================================================
class QFHttpMySqlDriverCreator : public QSqlDriverCreatorBase
{
	public:
		virtual QSqlDriver * createObject () const {return new QFHttpMySqlDriver();}
};

//======================================================
//                   TheApp
//======================================================
SqlJournal TheApp::f_sqlJournal;

TheApp::TheApp(int & argc, char ** argv)
	: QFApplication(argc, argv)
{
	f_reportProcessorSearchDirs = NULL;
	setApplicationName("qsqlmon");

	QSqlDatabase::registerSqlDriver("QFHTTPMYSQL", new QFHttpMySqlDriverCreator());

	QFXmlConfigSplittedFileLoader *ldr = new QFXmlConfigSplittedFileLoader(this);
	f_config = new QFXmlConfig(this);
	f_config->setConfigLoader(ldr);
	f_config->load();
}

TheApp::~TheApp()
{
	qfTrash() << QF_FUNC_NAME << "config()->dataDocument().isEmpty():" << config()->dataDocument().isEmpty();
	if(!config()->dataDocument().isEmpty()) config()->save();
}

TheApp* TheApp::instance()
{
	TheApp *a = qobject_cast<TheApp*>(QFApplication::instance());
	QF_ASSERT(a, "aplikace dosud neni inicializovana");
	return a;
}

QFXmlConfig* TheApp::config(bool throw_exc)
{
	Q_UNUSED(throw_exc);
	QFXmlConfig *ret = f_config;
	return ret;
}

void TheApp::redirectLog()
{
	bool log_to_file = config()->value("/log", "0").toBool();
	bool redirected = false;
	if(log_to_file) {
		QString fn = config()->value("/log/file", "err.log").toString();
		FILE *f = fopen(qPrintable(fn), "wb");
		if(f) {
			redirected = true;
			QFLog::redirectDefaultLogFile(f);
		}
	}
	if(!redirected) QFLog::redirectDefaultLogFile();
}

QString TheApp::versionString() const
{
	static QString s = "1.3.6";
	return s;
}

SqlJournal * TheApp::sqlJournal()
{
	return &f_sqlJournal;
}

QFSearchDirs* TheApp::reportProcessorSearchDirs()
{
	if(!f_reportProcessorSearchDirs) {
		f_reportProcessorSearchDirs = new QFSearchDirs();
	}
	return f_reportProcessorSearchDirs;
}
