#include "receipesplugin.h"
#include "../receipespartwidget.h"

#include <CardReader/cardreaderplugin.h>
#include <CardReader/checkedcard.h>

#include <qf/core/utils/treetable.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;

using namespace Receipes;

ReceipesPlugin::ReceipesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ReceipesPlugin::installed, this, &ReceipesPlugin::onInstalled);
}

void ReceipesPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	ReceipesPartWidget *pw = new ReceipesPartWidget(manifest()->featureId());
	framework->addPartWidget(pw);
}

CardReader::CardReaderPlugin *ReceipesPlugin::cardReaderPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

QVariantMap ReceipesPlugin::receipeTablesData(int card_id)
{
	QVariantMap ret;
	CardReader::CheckedCard cc = cardReaderPlugin()->checkCard(card_id);
	int run_id = cc.runId();
	{
		qf::core::model::SqlTableModel model;
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "*")
				.select2("runs", "*")
				.select2("classes", "name")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.from("runs")
				.join("runs.competitorId", "competitors.id")
				.join("competitors.classId", "classes.id")
				.where("runs.id=" QF_IARG(run_id));
		model.reload(qb.toString());
		qfu::TreeTable tt = model.toTreeTable("competitor");
		ret["competitor"] = tt.toVariant();
	}
	{
		qfu::TreeTable tt;
		//tt.appendColumn("position", QVariant::Int);
		//tt.appendColumn("code", QVariant::Int);
		//tt.appendColumn("stpTime", QVariant::String);
		//tt.appendColumn("lapTime", QVariant::String);
		QMapIterator<QString, QVariant> it(cc);
		while(it.hasNext()) {
			it.next();
			if(it.key() != QLatin1String("punches"))
				tt.setValue(it.key(), it.value());
		}
		//int no = 0;
		for(auto v : cc.punches()) {
			CardReader::CheckedPunch punch(v.toMap());
			qfu::TreeTableRow ttr = tt.appendRow();
			QMapIterator<QString, QVariant> it(punch);
			while(it.hasNext()) {
				it.next();
				ttr.setValue(it.key(), it.value());
			}
			//ttr.setValue("col1", ++no);
		}
		qfInfo() << tt.toString();
		ret["card"] = tt.toVariant();
	}
	return ret;
}

void ReceipesPlugin::previewReceipe(int card_id)
{
	QMetaObject::invokeMethod(this, "previewReceipeClassic", Qt::DirectConnection, Q_ARG(QVariant, card_id));
}


