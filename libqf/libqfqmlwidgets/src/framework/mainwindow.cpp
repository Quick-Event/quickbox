#include "mainwindow.h"
#include "application.h"
#include "pluginloader.h"
#include "dockwidget.h"
#include "partwidget.h"
#include "stackedcentralwidget.h"
#include "../menubar.h"
#include "../statusbar.h"
#include "../toolbar.h"
#include "../dialogs/qmldialog.h"

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/assert.h>
#include <qf/core/utils.h>
//#include <qf/core/settings.h>
//#include <qf/core/utils/crypt.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>

using namespace qf::qmlwidgets::framework;

MainWindow* MainWindow::self = nullptr;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	QMainWindow(parent, flags), IPersistentSettings(this)
{
	qfLogFuncFrame();
	Q_ASSERT(self == nullptr);
	self = this;

	m_pluginLoader = nullptr;

	Application *app = Application::instance();
	app->m_frameWork = this;
	QQmlEngine *qe = app->qmlEngine();
	qe->rootContext()->setContextProperty("FrameWork", this);
}

MainWindow::~MainWindow()
{
	qfLogFuncFrame();
	savePersistentSettings();
}

void MainWindow::loadPlugins()
{
	QF_SAFE_DELETE(m_pluginLoader);
	m_pluginLoader = new PluginLoader(this);
	connect(m_pluginLoader, &PluginLoader::loadingFinished, this, &MainWindow::pluginsLoaded, Qt::QueuedConnection);
	connect(this, &MainWindow::pluginsLoaded, this, &MainWindow::onPluginsLoaded);
	//connect(m_pluginLoader, &PluginLoader::loadingFinished, this, &MainWindow::whenPluginsLoaded, Qt::QueuedConnection);
	Application *app = qobject_cast<Application*>(QCoreApplication::instance());
	QJsonDocument profile = app->profile();
	QJsonArray arr = profile.object().value(QStringLiteral("plugins")).toObject().value(QStringLiteral("features")).toArray();
	QStringList feature_ids;
	Q_FOREACH(auto o, arr)
		feature_ids << o.toString();
	//QString ui_language_name;
	m_pluginLoader->loadPlugins(feature_ids);
}

void MainWindow::loadPersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		restoreGeometry(settings.value("geometry").toByteArray());
		restoreState(settings.value("state").toByteArray());
	}
}

void MainWindow::savePersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		settings.setValue("state", saveState());
		settings.setValue("geometry", saveGeometry());
	}
}

void MainWindow::showProgress(const QString &msg, int completed, int total)
{
	qfLogFuncFrame() << msg << completed << total;
	QCoreApplication::processEvents();
	emit progress(msg, completed, total);
}

void MainWindow::hideProgress()
{
	showProgress(QString(), 0, 0);
}
#ifdef GET_RESOURCE_IN_FRAMEWORK
QNetworkAccessManager *manager = new QNetworkAccessManager();
QMap<int, QNetworkReply*> servedReplies;
QMap<int, QByteArray> servedRepliesData;

int MainWindow::getResource(const QUrl &url, bool show_progress)
{
	static int request_count = 0;
	QNetworkRequest rq;
	rq.setUrl(url);
	//rq.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

	QNetworkReply *reply = manager->get(rq);
	if(show_progress) {
		showProgress(QString("%1/%2 %3").arg(0).arg(100).arg(reply->url().toString()), 0, 100);
	}
	int curr_request_id = ++request_count;
	servedReplies[curr_request_id] = reply;
	connect(reply, &QNetworkReply::readyRead, [curr_request_id]() {
		qfLogFuncFrame() << curr_request_id;
		QNetworkReply *rpl = servedReplies.value(curr_request_id);
		if(rpl) {
			QByteArray ba = rpl->readAll();
			qDebug() << rpl << ba;
			servedRepliesData[curr_request_id].append(ba);
		}
	});
	connect(reply, &QNetworkReply::downloadProgress, [show_progress, curr_request_id, this](qint64 _received, qint64 _total) {
		QNetworkReply *rpl = servedReplies.value(curr_request_id);
		if(rpl) {
			int completed = _received;
			int total = _total;
			if(total < 0) {
				total = 3 * _received;
				if(show_progress) {
					this->showProgress(QString("%1/%2 %3").arg(_received).arg(_total).arg(rpl->url().toString()), completed, total);
				}
			}
		}
	});
	connect(reply, &QNetworkReply::finished, [curr_request_id, this]() {
		QNetworkReply *rpl = servedReplies.value(curr_request_id);
		if(rpl) {
			if(rpl->error() == QNetworkReply::NoError) {
				QByteArray ba = servedRepliesData.value(curr_request_id);
				QString text = QString::fromUtf8(ba);
				emit getResourceContentFinished(curr_request_id, true, text);
			}
			else {
				emit getResourceContentFinished(curr_request_id, false, rpl->errorString());
			}
			rpl->deleteLater();
		}
	});
	/*
	connect(reply, &QNetworkReply::error, [](QNetworkReply::NetworkError code) {
		qfError() << "netwoork error";
	});
	*/
	//connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));
	return curr_request_id;
}
#endif
MainWindow *MainWindow::frameWork()
{
	Q_ASSERT(self != nullptr);
	return self;
}

bool MainWindow::setActivePart(const QString &feature_id)
{
	int ix = centralWidget()->featureToIndex(feature_id);
	if(ix < 0)
		return false;
	return centralWidget()->setActivePart(ix, true);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
	emit aboutToClose();
	ev->accept();
}

void MainWindow::onPluginsLoaded()
{
	//centralWidget()->setActivePart(0, true);
}

void MainWindow::setPersistentSettingDomains(const QString &organization_domain, const QString &organization_name, const QString &application_name)
{
	QCoreApplication::setOrganizationDomain(organization_domain);
	QCoreApplication::setOrganizationName(organization_name);
	if(!application_name.isEmpty()) {
		QCoreApplication::setApplicationName(application_name);
	}
	//QSettings::setDefaultFormat(QSettings::IniFormat);
}

qf::qmlwidgets::MenuBar *MainWindow::menuBar()
{
	QMenuBar *mb = Super::menuBar();
	MenuBar *menu_bar = qobject_cast<MenuBar*>(mb);
	if(!menu_bar) {
		QF_SAFE_DELETE(mb);
		menu_bar = new MenuBar(this);
		Super::setMenuBar(menu_bar);
	}
	return menu_bar;
}

qf::qmlwidgets::ToolBar *MainWindow::toolBar(const QString &name, bool create_if_not_exists)
{
	qf::qmlwidgets::ToolBar *ret = m_toolBars.value(name);
	if(!ret && !create_if_not_exists)
		return nullptr;
	ret = new qf::qmlwidgets::ToolBar(this);
	addToolBar(Qt::TopToolBarArea, ret);
	m_toolBars[name] = ret;
	return ret;
}

qf::qmlwidgets::StatusBar *MainWindow::statusBar()
{
	QStatusBar *sb = Super::statusBar();
	StatusBar *status_bar = qobject_cast<StatusBar*>(sb);
	if(!status_bar) {
		QF_SAFE_DELETE(sb);
		status_bar = new StatusBar(this);
		Super::setStatusBar(status_bar);
	}
	return status_bar;
}

void MainWindow::setStatusBar(qf::qmlwidgets::StatusBar *sbar)
{
	qfLogFuncFrame() << sbar << "previous:" << Super::statusBar();
	sbar->setParent(0);
	connect(this, SIGNAL(progress(QString,int,int)), sbar, SLOT(showProgress(QString,int,int)));
	Super::setStatusBar(sbar); /// deletes old status bar
	qfDebug() << Super::statusBar();
}

CentralWidget *MainWindow::centralWidget()
{
	QWidget *cw = Super::centralWidget();
	CentralWidget *central_widget = qobject_cast<CentralWidget*>(cw);
	if(!central_widget) {
		QF_SAFE_DELETE(cw);
		central_widget = new StackedCentralWidget(this);
		//central_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
		//central_widget->setStyleSheet("background:rgb(67, 67, 67)");
		Super::setCentralWidget(central_widget);
	}
	return central_widget;
}

void MainWindow::setCentralWidget(CentralWidget *widget)
{
	qfLogFuncFrame() << widget;
	widget->setParent(0);
	Super::setCentralWidget(widget);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget)
{
	qf::qmlwidgets::framework::IPersistentSettings* ps = dynamic_cast<qf::qmlwidgets::framework::IPersistentSettings*>(dockwidget);
	if(ps)
		ps->loadPersistentSettingsRecursively();
	dockwidget->setParent(0);
	Super::addDockWidget(area, dockwidget);
}

void MainWindow::addPartWidget(PartWidget *widget, const QString &feature_id)
{
	QF_ASSERT(widget != nullptr, "Widget is NULL", return);
	if(!feature_id.isEmpty()) {
		if(widget->featureId().isEmpty()) {
			qfDebug() << "setting" << widget << "featureId to:" << feature_id;
			widget->setFeatureId(feature_id);
		}
		else if(widget->featureId() != feature_id)
			qfWarning() << "different featureIds set:" << feature_id << "vs." << widget->featureId() << ", the second one will be used.";
	}
	if(widget->featureId().isEmpty())
		qfWarning() << widget << "adding part widget without featureId set can harm some default functionality.";
	//qfWarning() << widget->featureId() << widget;
	widget->loadPersistentSettingsRecursively();
	centralWidget()->addPartWidget(widget);
}

Plugin *MainWindow::plugin(const QString &feature_id, bool throw_exc)
{
	Plugin *ret = nullptr;
	if(m_pluginLoader) {
		ret = m_pluginLoader->loadedPlugins().value(feature_id);
	}
	if(!ret) {
		qfWarning() << "Plugin for feature id:" << feature_id << "is not installed!";
		qfWarning() << "Available feature ids:" << QStringList(m_pluginLoader->loadedPlugins().keys()).join(",");
		if(throw_exc)
			QF_EXCEPTION(tr("Plugin for feature id: '%1' is not installed!").arg(feature_id));
	}
	return ret;
}

Plugin *MainWindow::pluginForObject(QObject *qml_object)
{
	qfLogFuncFrame();
	Plugin *ret = nullptr;
	for(QObject *o = qml_object; o!=nullptr; o=o->parent()) {
		PartWidget *pw = qobject_cast<PartWidget*>(o);
		qfDebug() << o << "->" << pw;
		if(pw) {
			QString id = pw->featureId();
			ret = plugin(id);
			break;
		}
	}
	if(!ret)
		qfWarning() << "Cannot find plugin of:" << qml_object;
	return ret;
}

qf::qmlwidgets::dialogs::QmlDialog *MainWindow::createQmlDialog(QWidget *parent)
{
	if(parent == nullptr)
		parent = this;
	qf::qmlwidgets::dialogs::QmlDialog *ret = new qf::qmlwidgets::dialogs::QmlDialog(parent);
	//Application *app = Application::instance();
	//QQmlEngine *qe = app->qmlEngine();
	//qe->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	return ret;
}

#if 0
class TestObject : public QObject
{
	Q_OBJECT
public:
	TestObject(QObject *parent = nullptr) : QObject(parent)
	{
		static int cnt = 0;
		setObjectName(QString("OBJ%1").arg(++cnt));
		//QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
		qfInfo() << "CREATE" << this << "ownersip:" << QQmlEngine::objectOwnership(this);
	}
	~TestObject()
	{
		qfInfo() << "DESTROY" << this << "ownersip:" << QQmlEngine::objectOwnership(this);
	}
	Q_INVOKABLE QObject* parent() {return QObject::parent();}
};

QObject *MainWindow::obj_testing()
{
	QObject *ret = new TestObject();
	return ret;
}

//#include "mainwindow.moc"
#endif
