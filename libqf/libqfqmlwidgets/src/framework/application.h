#ifndef QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
#define QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H

#include "../qmlwidgetsglobal.h"

#include <QApplication>

class QQmlEngine;
class QQmlError;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT Application : public QApplication
{
	Q_OBJECT
private:
	typedef QApplication Super;
public:
	explicit Application(int & argc, char ** argv);
	~Application() Q_DECL_OVERRIDE;
public:
	virtual QQmlEngine* qmlEngine();
	/// Application doesn't take ownership of @a eng
	void setQmlEngine(QQmlEngine *eng);
	const QList<QQmlError>& qmlErrorList();
	void clearQmlErrorList();
	QStringList qmlPluginImportPaths() {return m_qmlPluginImportPaths;}
protected:
	virtual QQmlEngine* createQmlEngine();

	Q_SLOT void onQmlError(const QList<QQmlError> &qmlerror_list);
private:
	void releaseQmlEngine();
	void setupQmlImportPaths();
protected:
	QQmlEngine *m_qmlEngine;
	QStringList m_qmlLibraryImportPaths;
	QStringList m_qmlPluginImportPaths;
	QList<QQmlError> m_qmlErrorList;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
