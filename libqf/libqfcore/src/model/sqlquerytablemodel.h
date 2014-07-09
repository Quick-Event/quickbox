#ifndef QF_CORE_MODEL_SQLQUERYTABLEMODEL_H
#define QF_CORE_MODEL_SQLQUERYTABLEMODEL_H

#include "tablemodel.h"

namespace qf {
namespace core {
namespace model {

class QFCORE_DECL_EXPORT SqlQueryTableModel : public TableModel
{
	Q_OBJECT
private:
	typedef TableModel Super;
public:
	SqlQueryTableModel(QObject *parent = 0);
};

}}}

#endif // QF_CORE_MODEL_SQLQUERYTABLEMODEL_H
