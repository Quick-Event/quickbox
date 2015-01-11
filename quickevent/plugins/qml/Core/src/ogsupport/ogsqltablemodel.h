#ifndef OGSQLTABLEMODEL_H
#define OGSQLTABLEMODEL_H

#include "../qecorepluginglobal.h"

#include <qf/core/qml/sqltablemodel.h>

class QE_CORE_PLUGIN_DECL_EXPORT OGSqlTableModel : public qf::core::qml::SqlTableModel
{
	Q_OBJECT
private:
	typedef qf::core::qml::SqlTableModel Super;
public:
	OGSqlTableModel(QObject *parent = nullptr);
	~OGSqlTableModel();
public:
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
	bool setValue(int row, int column, const QVariant &val) Q_DECL_OVERRIDE;

	Q_INVOKABLE int ogTimeMsTypeId() const;
};

#endif // OGSQLTABLEMODEL_H
