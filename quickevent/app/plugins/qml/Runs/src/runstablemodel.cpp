#include "runstablemodel.h"

#include <quickevent/og/timems.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>

#include <QMimeData>

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{
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
	if(cd.matchesSqlId(QStringLiteral("startTimeMs"))) {
		flgs = Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | flgs;
		//qfInfo() << flgs;
	}
	return flgs;
}

QVariant RunsTableModel::value(int row_ix, int column_ix) const
{
	/*
	ColumnDefinition cd = columnDefinition(column_ix);
	if(cd.matchesSqlId(QStringLiteral("virtual.finishTimeMs"))) {
		QVariant ret;
		QVariant start_ms = value(row_ix, "startTimeMs");
		QVariant time_ms = value(row_ix, "timeMs");
		if(!start_ms.isNull() && !time_ms.isNull()) {
			ret = start_ms.toInt() + time_ms.toInt();
		}
		return ret;
	}
	*/
	return Super::value(row_ix, column_ix);
}

bool RunsTableModel::setValue(int row_ix, int column_ix, const QVariant &val)
{
	bool ret;
	ColumnDefinition cd = columnDefinition(column_ix);
	if(cd.matchesSqlId(QStringLiteral("finishTimeMs"))) {
		QVariant start_ms = value(row_ix, "startTimeMs");
		if(!start_ms.isNull()) {
			int time_ms = val.toInt() - start_ms.toInt();
			if(time_ms > 0) {
				Super::setValue(row_ix, "timeMs", time_ms);
			}
		}
	}
	else if(cd.matchesSqlId(QStringLiteral("timeMs"))) {
		QVariant start_ms = value(row_ix, "startTimeMs");
		if(!start_ms.isNull()) {
			int finish_ms = val.toInt() + start_ms.toInt();
			if(finish_ms > 0) {
				Super::setValue(row_ix, "finishTimeMs", finish_ms);
			}
		}
	}
	else if(cd.matchesSqlId(QStringLiteral("startTimeMs"))) {
		if(!val.isNull()) {
			int start_ms = val.toInt();
			int finish_ms = value(row_ix, "finishTimeMs").toInt();
			int time_ms = value(row_ix, "timeMs").toInt();
			if(finish_ms > 0) {
				int time_ms = finish_ms - start_ms;
				Super::setValue(row_ix, "timeMs", time_ms);
			}
			else if(time_ms > 0) {
				finish_ms = start_ms + time_ms;
				Super::setValue(row_ix, "finishTimeMs", finish_ms);
			}
		}
	}
	ret = Super::setValue(row_ix, column_ix, val);
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
		qf::core::sql::Transaction transaction(sqlConnection());
		quickevent::og::TimeMs t1 = v1.value<quickevent::og::TimeMs>();
		quickevent::og::TimeMs t2 = v2.value<quickevent::og::TimeMs>();
		int msec1 = -1, msec2 = -1;
		qf::core::sql::Query q(transaction.connection());
		QString qs = "SELECT id, startTimeMs FROM runs WHERE id IN (" QF_IARG(id1) ", " QF_IARG(id2) ") FOR UPDATE";
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
			postRow(r1, true);
			postRow(r2, true);
			transaction.commit();
		}
		else {
			err_msg = tr("Mid-air collision switching start times, reload table and try it again.");
		}
	}
	emit startTimesSwitched(id1, id2, err_msg);
}

bool RunsTableModel::postRow(int row_no, bool throw_exc)
{
	bool is_single_user = sqlConnection().driverName().endsWith(QLatin1String("SQLITE"), Qt::CaseInsensitive);
	if(is_single_user)
		return Super::postRow(row_no, throw_exc);

	int col_stime = columnIndex("startTimeMs");
	QF_ASSERT(col_stime >= 0, "Bad startTimeMs column!", return false);
	if(isDirty(row_no, col_stime)) {
		int id = value(row_no, "runs.id").toInt();
		int orig_msec = origValue(row_no, col_stime).toInt();
		int db_msec = 0;

		qf::core::sql::Transaction transaction(sqlConnection());
		QString qs = "SELECT id, startTimeMs FROM runs WHERE id=" QF_IARG(id) " FOR UPDATE";
		qf::core::sql::Query q(transaction.connection());
		q.exec(qs, qf::core::Exception::Throw);
		if(q.next()) {
			db_msec = q.value("startTimeMs").toInt();
		}
		if(orig_msec == db_msec) {
			bool ret = Super::postRow(row_no, throw_exc);
			transaction.commit();
			return ret;
		}
		else {
			QString err_msg = tr("Mid-air collision setting start time, reload table and try it again.");
			revertRow(row_no);
			if(throw_exc)
				QF_EXCEPTION(err_msg);
			return false;
		}
	}
	return Super::postRow(row_no, throw_exc);
}

