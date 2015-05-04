#include "runstablemodel.h"

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{

}

QVariant RunsTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
	return ret;
}

