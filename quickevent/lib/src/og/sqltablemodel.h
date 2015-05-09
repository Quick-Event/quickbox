#ifndef QUICKEVENT_OG_SQLTABLEMODEL_H
#define QUICKEVENT_OG_SQLTABLEMODEL_H

#include "../quickeventglobal.h"

#include <qf/core/model/sqltablemodel.h>

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT SqlTableModel : public qf::core::model::SqlTableModel
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlTableModel Super;
public:
	SqlTableModel(QObject *parent = nullptr);
	~SqlTableModel();
public:
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
	using Super::value;
	bool setValue(int row, int column, const QVariant &val) Q_DECL_OVERRIDE;

	//Q_INVOKABLE int ogTimeMsTypeId() const;
};

}}

#endif // QUICKEVENT_OG_SQLTABLEMODEL_H
