#include "runstablemodel.h"

#include <QMimeData>

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{

}

QVariant RunsTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
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
	int col = columnIndex("startTimeMs");
	QF_ASSERT(col >= 0, "Bad column!", return);

	QModelIndex ix1 = index(r1, col);
	QVariant v1 = ix1.data(Qt::EditRole);
	QModelIndex ix2 = index(r2, col);
	QVariant v2 = ix2.data(Qt::EditRole);
	setData(ix1, v2);
	setData(ix2, v1);
	postRow(r1, true);
	postRow(r2, true);
	emit startTimesSwitched(r1, r2);
}

