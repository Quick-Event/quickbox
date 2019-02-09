#include "runstablemodel.h"
#include "Runs/runsplugin.h"

#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/siid.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <QMimeData>

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT(plugin != nullptr, "Runs plugin not installed!", return nullptr);
	return plugin;
}

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{
	clearColumns(col_COUNT);
	setColumn(col_runs_isRunning, ColumnDefinition("runs.isRunning", tr("Runnig")));
	setColumn(col_runs_id, ColumnDefinition("runs.id", tr("id")).setReadOnly(true));
	setColumn(col_relays_name, ColumnDefinition("relayName", tr("Relay")));
	setColumn(col_runs_leg, ColumnDefinition("runs.leg", tr("Leg")));
	setColumn(col_classes_name, ColumnDefinition("classes.name", tr("Class")));
	setColumn(col_startNumber, ColumnDefinition("startNumber", tr("SN", "start number")).setToolTip(tr("Start number")));
	setColumn(col_competitors_siId, ColumnDefinition("competitors.siId", tr("SI")).setToolTip(tr("Registered SI")).setReadOnly(true));
	setColumn(col_competitorName, ColumnDefinition("competitorName", tr("Name")));
	setColumn(col_registration, ColumnDefinition("registration", tr("Reg")));
	setColumn(col_runs_license, ColumnDefinition("licence", tr("Lic")).setToolTip(tr("License")));
	setColumn(col_runs_ranking, ColumnDefinition("ranking", tr("Rank")).setToolTip(tr("Ranking")));
	setColumn(col_runs_siId, ColumnDefinition("runs.siId", tr("SI")).setToolTip(tr("Actual SI")).setCastType(qMetaTypeId<quickevent::core::si::SiId>()));
	setColumn(col_runs_checkDateTime, ColumnDefinition("checktimes.punchDateTime", tr("Check")).setToolTip(tr("Time loaded from CHECK station")));
	setColumn(col_runs_startTimeMs, ColumnDefinition("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()));
	setColumn(col_runs_timeMs, ColumnDefinition("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()));
	setColumn(col_runs_finishTimeMs, ColumnDefinition("runs.finishTimeMs", tr("Finish")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()));
	setColumn(col_runs_notCompeting, ColumnDefinition("runs.notCompeting", tr("NC")).setToolTip(tr("Not competing")));
	setColumn(col_runs_cardRentRequested, ColumnDefinition("runs.cardLent", tr("LR")).setToolTip(tr("Card rent requested")));
	setColumn(col_cardInLentTable, ColumnDefinition("cardInLentTable", tr("LT", "cardInLentTable")).setToolTip(tr("Card in lent table")));
	setColumn(col_runs_cardReturned, ColumnDefinition("runs.cardReturned", tr("R")).setToolTip(tr("Card returned")));
	setColumn(col_disqReason, ColumnDefinition("disqReason", tr("Error")).setToolTip(tr("Disqualification reason")).setReadOnly(true));
	setColumn(col_runs_disqualified, ColumnDefinition("runs.disqualified", tr("DISQ")).setToolTip(tr("Disqualified")));
	setColumn(col_competitors_note, ColumnDefinition("competitors.note", tr("Note")));

	connect(this, &RunsTableModel::dataChanged, this, &RunsTableModel::onDataChanged, Qt::QueuedConnection);
}

QVariant RunsTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret;
	ret = Super::data(index, role);
	return ret;
}

Qt::ItemFlags RunsTableModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flgs = Super::flags(index);
	ColumnDefinition cd = columnDefinition(index.column());
	if(index.column() == col_runs_startTimeMs) {
		flgs = Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | flgs;
		//qfInfo() << flgs;
	}
	return flgs;
}

QVariant RunsTableModel::value(int row_ix, int column_ix) const
{
	if(column_ix == col_disqReason) {
		qf::core::utils::TableRow row = tableRow(row_ix);
		bool mis_punch = row.value(QStringLiteral("runs.misPunch")).toBool();
		bool bad_check = row.value(QStringLiteral("runs.badCheck")).toBool();
		QStringList sl;
		if(mis_punch)
			sl << tr("MisPunch");
		if(bad_check)
			sl << tr("BadCheck");
		if(sl.isEmpty())
			return QStringLiteral(" ");
		else
			return sl.join(',');
	}
	else if(column_ix == col_runs_isRunning) {
		bool is_running = Super::value(row_ix, column_ix).toBool();
		return is_running;
	}
	return Super::value(row_ix, column_ix);
}

bool RunsTableModel::setValue(int row_ix, int column_ix, const QVariant &val)
{
	//qfInfo() << column_ix << val << val.typeName() << "is null:" << val.isNull();
	if(column_ix == col_runs_isRunning) {
		bool is_running = val.toBool();
		if(!is_running) {
			int finish_ms = value(row_ix, col_runs_finishTimeMs).toInt();
			if(finish_ms > 0) {
				emit badDataInput(tr("Canont set not running flag for competitor with valid finish time."));
				return false;
			}
		}
		return Super::setValue(row_ix, column_ix, is_running? is_running: QVariant());
	}
	if(column_ix == col_runs_finishTimeMs) {
		QVariant start_ms = value(row_ix, col_runs_startTimeMs);
		if(!start_ms.isNull()) {
			int time_ms = val.toInt() - start_ms.toInt();
			if(time_ms > 0) {
				Super::setValue(row_ix, col_runs_timeMs, time_ms);
			}
			else {
				Super::setValue(row_ix, col_runs_timeMs, QVariant());
			}
		}
	}
	else if(column_ix == col_runs_timeMs) {
		QVariant start_ms = value(row_ix, col_runs_startTimeMs);
		if(!start_ms.isNull()) {
			int finish_ms = val.toInt() + start_ms.toInt();
			if(finish_ms > 0) {
				Super::setValue(row_ix, col_runs_finishTimeMs, finish_ms);
			}
			else {
				Super::setValue(row_ix, col_runs_finishTimeMs, QVariant());
			}
		}
	}
	else if(column_ix == col_runs_startTimeMs) {
		if(!val.isNull()) {
			int start_ms = val.toInt();
			int finish_ms = value(row_ix, col_runs_finishTimeMs).toInt();
			int time_ms = value(row_ix, col_runs_timeMs).toInt();
			if(finish_ms > 0) {
				int time_ms = finish_ms - start_ms;
				Super::setValue(row_ix, col_runs_timeMs, time_ms);
			}
			else if(time_ms > 0) {
				finish_ms = start_ms + time_ms;
				Super::setValue(row_ix, col_runs_finishTimeMs, finish_ms);
			}
		}
	}
	bool ret = Super::setValue(row_ix, column_ix, val);
	return ret;
}

static auto MIME_TYPE = QStringLiteral("application/quickevent.startTime");

QStringList RunsTableModel::mimeTypes() const
{
	QStringList types;
	types << MIME_TYPE;
	return types;
}

QMimeData *RunsTableModel::mimeData(const QModelIndexList &indexes) const
{
	qfLogFuncFrame();
	QMimeData *mimeData = new QMimeData();
	QByteArray encoded_data = QString::number(indexes.value(0).row()).toUtf8();
	mimeData->setData(MIME_TYPE, encoded_data);
	return mimeData;
}

Qt::DropActions RunsTableModel::supportedDropActions() const
{
	//qfLogFuncFrame();
	return Qt::MoveAction;// | Qt::CopyAction;
}
/*
bool RunsTableModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
	qfLogFuncFrame() << "row:" << row << "col:" << column << action << parent;
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(parent);

	if (!data->hasFormat(MIME_TYPE))
		return false;

	if (column > 0)
		return false;
	qfInfo() << "TRUE";
	return true;
}
*/
bool RunsTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	qfLogFuncFrame() << "row:" << row << "col:" << column << "parent:" << parent;
	if (!canDropMimeData(data, action, row, column, parent))
		return false;

	if (action == Qt::IgnoreAction)
		return true;
	if (!parent.isValid()) {
		// not drop on item
		return false;
	}
	int r1 = QString::fromUtf8(data->data(MIME_TYPE)).toInt();
	int r2 = parent.row();
	qfDebug() << "switch:" << r1 << "and" << r2;
	switchStartTimes(r1, r2);
	return true;
}

void RunsTableModel::switchStartTimes(int r1, int r2)
{
	qfLogFuncFrame() << r1 << r2;
	int col_stime = columnIndex("startTimeMs");
	QF_ASSERT(col_stime >= 0, "Bad startTimeMs column!", return);

	int id1 = value(r1, "runs.id").toInt();
	int id2 = value(r2, "runs.id").toInt();
	QString err_msg;
	QModelIndex ix1 = index(r1, col_stime);
	QVariant v1 = ix1.data(Qt::EditRole);
	QModelIndex ix2 = index(r2, col_stime);
	QVariant v2 = ix2.data(Qt::EditRole);

	bool is_single_user = sqlConnection().driverName().endsWith(QLatin1String("SQLITE"), Qt::CaseInsensitive);
	if(is_single_user) {
		setData(ix1, v2);
		setData(ix2, v1);
		postRow(r1, true);
		postRow(r2, true);
	}
	else {
		//qf::core::sql::Transaction transaction(sqlConnection());
		quickevent::core::og::TimeMs t1 = v1.value<quickevent::core::og::TimeMs>();
		quickevent::core::og::TimeMs t2 = v2.value<quickevent::core::og::TimeMs>();
		int msec1 = -1, msec2 = -1;
		qf::core::sql::Query q(sqlConnection());
		QString qs = "SELECT id, startTimeMs FROM runs WHERE id IN (" QF_IARG(id1) ", " QF_IARG(id2) ")";
		q.exec(qs, qf::core::Exception::Throw);
		while(q.next()) {
			int id = q.value("id").toInt();
			if(id == id1)
				msec1 = q.value("startTimeMs").toInt();
			else if(id == id2)
				msec2 = q.value("startTimeMs").toInt();
		}
		qfDebug() << t1.msec() << msec1 << t2.msec() << msec2;
		if(msec1 == t1.msec() && msec2 == t2.msec()) {
			setData(ix1, v2);
			setData(ix2, v1);
			postRow(r1, qf::core::Exception::Throw);
			postRow(r2, qf::core::Exception::Throw);
			//transaction.commit();
		}
		else {
			err_msg = tr("Mid-air collision switching start times, reload table and try it again.");
		}
	}
	emit startTimesSwitched(id1, id2, err_msg);
}

void RunsTableModel::onDataChanged(const QModelIndex &top_left, const QModelIndex &bottom_right, const QVector<int> &roles)
{
	Q_UNUSED(roles)
	if(top_left.column() <= RunsTableModel::col_runs_siId && bottom_right.column() >= RunsTableModel::col_runs_siId)
		emit runnerSiIdEdited();
}

bool RunsTableModel::postRow(int row_no, bool throw_exc)
{
	bool is_single_user = sqlConnection().driverName().endsWith(QLatin1String("SQLITE"), Qt::CaseInsensitive);
	if(is_single_user)
		return Super::postRow(row_no, throw_exc);

	if(isDirty(row_no, col_runs_startTimeMs)) {
		int run_id = value(row_no, col_runs_id).toInt();
		int orig_msec = origValue(row_no, col_runs_startTimeMs).toInt();
		int db_msec = 0;

		//qf::core::sql::Transaction transaction();
		QString qs = "SELECT id, startTimeMs FROM runs WHERE id=" QF_IARG(run_id); // " FOR UPDATE";
		qf::core::sql::Query q(sqlConnection());
		q.exec(qs, qf::core::Exception::Throw);
		if(q.next()) {
			db_msec = q.value("startTimeMs").toInt();
		}
		if(orig_msec == db_msec) {
			bool ret = Super::postRow(row_no, throw_exc);
			//transaction.commit();
			QVariant v = value(row_no, col_runs_finishTimeMs);
			if(!v.isNull()) {
				runsPlugin()->reloadTimesFromCard(run_id);
				reloadRow(row_no);
			}
			return ret;
		}
		else {
			QString err_msg = tr("Mid-air collision setting start time, reload table and try it again.");
			setValue(row_no, col_runs_startTimeMs, db_msec);
			if(throw_exc)
				QF_EXCEPTION(err_msg);
			return false;
		}
	}
	return Super::postRow(row_no, throw_exc);
}

