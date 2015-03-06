#ifndef QUICKEVENT_OGSQLTABLEMODEL_H
#define QUICKEVENT_OGSQLTABLEMODEL_H

#include "../quickeventglobal.h"

#include <qf/core/model/sqltablemodel.h>

class QUICKEVENT_DECL_EXPORT OGSqlTableModel : public qf::core::model::SqlTableModel
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlTableModel Super;
public:
	OGSqlTableModel(QObject *parent = nullptr);
	~OGSqlTableModel();
public:
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
	bool setValue(int row, int column, const QVariant &val) Q_DECL_OVERRIDE;

	Q_INVOKABLE int ogTimeMsTypeId() const;
};

#endif // QUICKEVENT_OGSQLTABLEMODEL_H
