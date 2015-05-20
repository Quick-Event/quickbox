
//
// Author: Frantisek Vacek <fanda.vacek@gmail.com>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef THEAPP_H
#define THEAPP_H

#include <qf/core/assert.h>
#include <qf/core/utils/crypt.h>
#include <qf/core/utils.h>

//#include <qfappreportsearchdirsinterface.h>

#include <QApplication>

class SqlJournal //: public qf::core::LogDevice
{
protected:
	QStringList m_log;
public:
	virtual void log(const QString &msg);

	QStringList content() {return m_log;}
	void setContent(const QStringList &sl) {m_log = sl;}
};

//! TODO: write class documentation.
class  TheApp : public QApplication //, public QFAppReportSearchDirsInterface
{
	Q_OBJECT
public:
	TheApp(int & argc, char ** argv);
	~TheApp() Q_DECL_OVERRIDE;

	// ie: driver=&user=foo&passwordbar&host=baz&port=0&database=quickevent
	QF_PROPERTY_IMPL(QString, o, O, neTimeConnectionSettings)
protected:
	static SqlJournal f_sqlJournal;
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
	qf::core::utils::Crypt crypt();
};

inline TheApp* theApp() {return TheApp::instance();}

template <class T>
T qfFindParent(const QObject *_o)
{
	T t = NULL;
	QObject *o = const_cast<QObject*>(_o);
	while(o) {
		o = o->parent();
		if(!o) break;
		t = qobject_cast<T>(o);
		if(t) break;
	}
	QF_ASSERT_EX(t!=nullptr, QString("object 0x%1 has not any parent of requested type.").arg((ulong)_o, 0, 16));
	return t;
}

#endif // THEAPP_H

