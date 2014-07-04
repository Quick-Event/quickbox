#ifndef QF_CORE_UTILS_H
#define QF_CORE_UTILS_H

#include "coreglobal.h"
#include <QVariant>

#define QF_SAFE_DELETE(x) if(x != nullptr) {delete x; x = nullptr;}

#define QF_SARG(s) "'" + QString(s) + "'"
#define QF_IARG(i) "" + QString::number(i) + ""

class QString;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Utils
{
public:
	static void parseFieldName(const QString& full_field_name, QString *pfield_name = NULL, QString *ptable_name = NULL, QString *pdb_name = NULL);
	static bool fieldNameEndsWith(const QString &field_name1, const QString &field_name2);
	static QVariant retypeVariant(const QVariant &_val, QVariant::Type type);
};

}}

#endif
