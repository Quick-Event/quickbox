#ifndef QUICKEVENTCORE_OG_SQLTABLEMODEL_H
#define QUICKEVENTCORE_OG_SQLTABLEMODEL_H

#include "../quickeventcoreglobal.h"

#include <qf/core/model/sqltablemodel.h>

namespace quickevent {
namespace core {
namespace og {

class QUICKEVENTCORE_DECL_EXPORT SqlTableModel : public qf::core::model::SqlTableModel
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlTableModel Super;
public:
	SqlTableModel(QObject *parent = nullptr);
	~SqlTableModel();
public:
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
protected:
	QVariant rawValueToEdit(int column_index, const QVariant &val) const Q_DECL_OVERRIDE;
	QVariant editValueToRaw(int column_index, const QVariant &val) const Q_DECL_OVERRIDE;
};

}}}

#endif // QUICKEVENTCORE_OG_SQLTABLEMODEL_H
