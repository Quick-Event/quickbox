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
};

#endif // OGSQLTABLEMODEL_H
