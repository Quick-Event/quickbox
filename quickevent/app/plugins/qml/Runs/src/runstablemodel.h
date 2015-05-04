#ifndef RUNSTABLEMODEL_H
#define RUNSTABLEMODEL_H

#include <quickevent/og/sqltablemodel.h>

class RunsTableModel : public quickevent::og::SqlTableModel
{
	Q_OBJECT
private:
	typedef quickevent::og::SqlTableModel Super;
public:
	RunsTableModel(QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // RUNSTABLEMODEL_H
