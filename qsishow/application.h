#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QVariant>
#include <QtSql/QSqlQuery>

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int & argc, char ** argv);

	static Application* instance();
	QSqlDatabase sqlConnetion();
	QSqlQuery execSql(const QString &query_str);
	/// nevim proc, ale z QML nemuzu na properties objektu primo
	Q_INVOKABLE QVariant getProperty(const QString &prop_name) {return property(prop_name.toLatin1().constData());}
	QVariantMap sqlRecordToMap(const QSqlRecord &rec);
protected:
	QVariant getCmdLineArgValue(const QString &long_key, const QString &short_key = QString());
	void initSettings();
	void initConfigValue(const QString &settings_path, const QString &cli_long_key, const QVariant &default_val = QVariant());
	//QString recodeSqlString(const QString &str);
signals:
    
public slots:
	QVariant eventInfo();
	QVariant setting(const QString &path, const QVariant &default_val = QVariant());
	void setSetting(const QString &path, const QVariant &val);
	void flushConfig();
	QVariant appConfigValue(const QString &path, const QVariant def_val = QVariant());
	QString profile() {return appConfigValue("application/profile").toString();}
protected:
	QVariantMap f_appConfig;
};

#endif // APPLICATION_H
