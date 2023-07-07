#ifndef QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
#define QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H

#include "../qmlwidgetsglobal.h"

#include <QApplication>
#include <QJsonDocument>
#include <QQmlError>

class QQmlEngine;

namespace qf {
namespace qmlwidgets {
namespace framework {

class MainWindow;

class QFQMLWIDGETS_DECL_EXPORT Application : public QApplication
{
	Q_OBJECT
	friend class MainWindow;
private:
	typedef QApplication Super;
public:
	explicit Application(int & argc, char ** argv);
	~Application() Q_DECL_OVERRIDE;
public:
	static Application* instance(bool must_exist = true);
	QString pluginDataDir() {return m_pluginDataDir;}
	MainWindow* frameWork();

	void loadStyleSheet(const QString &file = QString());
public slots:
	QString applicationDirPath();
	QString applicationName();
	QStringList arguments();
protected:
	QJsonDocument profile();
protected:
	QString m_pluginDataDir;
	QJsonDocument m_profile;
	bool m_profileLoaded = false;
	MainWindow* m_frameWork = nullptr;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_APPLICATION_H
