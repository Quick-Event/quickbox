#ifndef QF_CORE_UTILS_H
#define QF_CORE_UTILS_H

#include "coreglobal.h"
#include <QVariant>

#define QF_SAFE_DELETE(x) if(x != nullptr) {delete x; x = nullptr;}

#define QF_SARG(s) "'" + QString(s) + "'"
#define QF_IARG(i) "" + QString::number(i) + ""

#define QF_QUOTEME(x) QStringLiteral(#x)

#define QF_OPTION_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(value(QF_QUOTEME(getter_prefix##name_rest)));} \
	public: void setter_prefix##name_rest(const ptype &val) {(*this)[QF_QUOTEME(getter_prefix##name_rest)] = val;}
/// for default values other than QVariant()
#define QF_OPTION_FIELD2_RW(ptype, getter_prefix, setter_prefix, name_rest, default_value) \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(value(QF_QUOTEME(getter_prefix##name_rest), default_value));} \
	public: void setter_prefix##name_rest(const ptype &val) {(*this)[QF_QUOTEME(getter_prefix##name_rest)] = val;}

/// for implicitly shared classes properties
#define QF_SHARED_CLASS_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const {return d->getter_prefix##name_rest;} \
	public: void setter_prefix##name_rest(const ptype &val) {d->getter_prefix##name_rest = val;}

class QString;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Utils
{
public:
	static void parseFieldName(const QString& full_field_name, QString *pfield_name = NULL, QString *ptable_name = NULL, QString *pdb_name = NULL);
	/// @returns: True if @a field_name1 ends with @a field_name2. Comparision is case insensitive
	static bool fieldNameEndsWith(const QString &field_name1, const QString &field_name2);
	static bool fieldNameCmp(const QString &fld_name1, const QString &fld_name2);
	static QVariant retypeVariant(const QVariant &_val, QVariant::Type type);
};

}}

#endif
