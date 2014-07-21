
//
// Author: Frantisek Vacek <fanda.vacek@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef THEAPP_H
#define THEAPP_H

#include <qf/core/logdevice.h>

//#include <qfappreportsearchdirsinterface.h>

#include <QApplication>

class SqlJournal //: public qf::core::LogDevice
{
	protected:
		QStringList m_log;
	public:
		virtual void log(const QString &msg)
		{
			m_log << msg;
		}

		QStringList content() {return m_log;}
		void setContent(const QStringList &sl) {m_log = sl;}
};

//! TODO: write class documentation.
class  TheApp : public QApplication //, public QFAppReportSearchDirsInterface
{
	Q_OBJECT
	protected:
		static SqlJournal f_sqlJournal;
		//QFSearchDirs *f_reportProcessorSearchDirs;
		//QFXmlConfig *f_config;
	public:
		SqlJournal* sqlJournal();
	public:
		//virtual QFXmlConfig* appConfig(bool throw_exc = Qf::ThrowExc) {Q_UNUSED(throw_exc); return config();}
		//virtual QFXmlConfig* config(bool throw_exc = Qf::ThrowExc);
		/// nastavi logovani do souboru uvedeneho v configu nebo to stderr.
		//void redirectLog();
		QString versionString() const;

		static TheApp* instance();
		//virtual QFSearchDirs *reportProcessorSearchDirs();
	public:
		TheApp(int & argc, char ** argv);
		virtual ~TheApp();
};

inline TheApp* theApp() {return TheApp::instance();}

#endif // THEAPP_H

