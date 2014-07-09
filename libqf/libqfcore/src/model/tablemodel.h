#ifndef QF_CORE_MODEL_TABLEMODEL_H
#define QF_CORE_MODEL_TABLEMODEL_H

#include "../core/coreglobal.h"

#include <QAbstractTableModel>

namespace qf {
namespace core {
namespace model {

class QFCORE_DECL_EXPORT TableModel : public QAbstractTableModel
{
	Q_OBJECT
private:
	typedef QAbstractTableModel Super;
public:
	explicit TableModel(QObject *parent = 0);

signals:

public slots:

};

}}}

#endif // QF_CORE_MODEL_TABLEMODEL_H
