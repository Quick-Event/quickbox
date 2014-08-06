#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_centralwidget.h"

#include <qf/core/assert.h>
#include <qf/core/sql/connection.h>
#include <qf/qmlwidgets/framework/ipersistentsettings.h>

#include <QMap>
#include <QMainWindow>

class ServerTreeModel;
class QAction;
class QLabel;
class QMenu;
class QTextEdit;
//class QUdpSocket;
class QModelIndex;
class ServerTreeDock;
class SqlDock;
class QFXmlConfigDocument;
class Connection;
class Database;
class QFSqlQueryTableModel;
class QSpinBox;

namespace qf {
namespace core {
namespace model {
class SqlQueryTableModel;
}
}
}

class MainWindow : public QMainWindow, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
protected:
	//bool event(QEvent *event);
	virtual void closeEvent(QCloseEvent *e);
	//virtual void showEvent(QShowEvent * e);
	virtual void changeEvent(QEvent * e);
	//virtual void focusInEvent(QFocusEvent *e);

	void executeSqlLines(const QString &lines);

	void setStatusText(const QString& s, int where = 0);

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
public slots:
	void appendInfo(const QString &s);
	void setProgressValue(double val, const  QString &label_text = QString());
private slots:
	void treeNodeCollapsed(const QModelIndex &index);
	void treeNodeExpanded(const QModelIndex &index);
	void treeNodeDoubleClicked(const QModelIndex &index);
	void treeServersContextMenuRequest(const QPoint&);

	void executeSql();
	void executeSelectedLines();
	void wordWrapSqlEditor(bool checked);
	void setSqlDelimiter();
	void executeSqlScript();
	void showSqlJournal();
	void configure();
	void about();
	void aboutQt();
	void changeLog();
	void mysqlSyntax();
	void sqliteSyntax();
	void availableDrivers();
	void checkDrivers();
	void tearOffTable();

	void addServer(Connection *connection_to_copy = NULL);
	void setDbSearchPath(const QString &path);

	void tableStatusBarTextAction(const QString &text);

	void lazyInit();
private:
	Ui::CentralWidget ui;

	ServerTreeDock *serverDock;
	SqlDock *sqlDock;

	QChar f_sqlDelimiter;
private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void createDockWindows();
	void init();

	QMenu *menuFile;
	QMenu *menuHelp;
	QMenu *menuView;

	QSpinBox *edLimit;

	QMap<QString, QAction*> actionMap;

	QList<QPointer<QDialog> > tearOffDialogs;

	qf::core::model::SqlQueryTableModel* queryViewModel();
	void setQueryViewModel(qf::core::model::SqlQueryTableModel *m);
	//QList<QWidget*> statusBarWidgets;
private:
	QAction* action(const QString& action_name) {
		QAction *a = actionMap.value(action_name);
		QF_ASSERT(a!=nullptr,
				  QString("Action '%1' not found !").arg(action_name),
				  return a);
		return a;
	}
	QString f_activeSetNames;
	/// database z niz se provadi dotazy
	qf::core::sql::Connection m_activeConnection;
	/// \return previously active connection
	qf::core::sql::Connection setActiveConnection1(const qf::core::sql::Connection &c);
	qf::core::sql::Connection setActiveConnection2(Database *d);
	
	bool execQuery(const QString& query_str);
public:
	/**
	 *  throw exception if activeConnection is not valid.
	 */
	qf::core::sql::Connection activeConnection();
	bool execCommand(const QString& query_str);
public:
	MainWindow();
	~MainWindow();
};

#endif
