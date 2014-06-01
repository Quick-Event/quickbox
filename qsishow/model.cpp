#include "model.h"
#include "application.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QStringBuilder>
#include <QDebug>

Model::Model(QObject *parent) :
	QObject(parent)
{
	f_shiftOffset = -1;
}

void Model::shift()
{
	f_shiftOffset++;
	//qDebug() << "shift offset:" << f_shiftOffset;
}

QVariant Model::data(int index)
{
	QVariant ret;
	while((f_shiftOffset + index) >= f_storage.count()) {
		if(!addCategoryToStorage()) break;
	}
	int ix = f_shiftOffset + index;
	if(ix < f_storage.count()) {
		ret = f_storage[ix];
	}
	return ret;
}

void Model::reloadCategories()
{
	f_categoriesToProceed.clear();
	Application *app = Application::instance();
	QString qs = app->appConfigValue("profile/"%app->profile()%"/query/categories").toString();
	QSqlQuery q = app->execSql(qs);
	while(q.next()) {
		f_categoriesToProceed << q.value(0).toString();
	}
}

bool Model::addCategoryToStorage()
{
	/// ltrim f_storage
	f_storage = f_storage.mid(f_shiftOffset);
	f_shiftOffset = 0;

	if(f_categoriesToProceed.isEmpty()) {reloadCategories();}
	if(f_categoriesToProceed.isEmpty()) {
		qCritical() << "Categories load ERROR";
		return false;
	}
	Application *app = Application::instance();
	QString cat_to_load = f_categoriesToProceed.takeFirst();
	QVariantMap m_category;
	{
		QString qs = app->appConfigValue("profile/"%app->profile()%"/query/category").toString();
		qs.replace("{{CATEGORY}}", cat_to_load);
		QSqlQuery q = app->execSql(qs);
		if(q.next()) {
			QSqlRecord rec = q.record();
			QVariantMap m;
			m_category = app->sqlRecordToMap(rec);
			m["type"] = "category";
			m["category"] = m_category;
			f_storage << m;
		}
		else {
			qCritical() << "Entry for classname" << cat_to_load << "does not exist !!!";
		}
	}
	{
		QString qs = app->appConfigValue("profile/"%app->profile()%"/query/details").toString();
		qs.replace("{{CATEGORY}}", cat_to_load);
		QSqlQuery q = app->execSql(qs);
		int pos = 0;
		while(q.next()) {
			QSqlRecord rec = q.record();
			QVariantMap m;
			QVariantMap m_detail = app->sqlRecordToMap(rec);
			m_detail["pos"] = ++pos;
			m["type"] = "detail";
			m["detail"] = m_detail;
			/// pridej k detailu i kategorii, protoze na prvnim miste listu se zobrazuje vzdy zahlavi aktualni kategorie kvuli prehlednosti
			m["category"] = m_category;
			f_storage << m;
		}
	}
	return true;
}

