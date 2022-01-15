#ifndef QF_CORE_UTILS_H
#define QF_CORE_UTILS_H

#include "coreglobal.h"
#include "exception.h"

#include <QVariant>

#define QF_AS_CONST(obj) const_cast<const decltype(obj) &>(obj)

#define QF_SAFE_DELETE(x) if(x != nullptr) {delete x; x = nullptr;}

#define QF_CARG(s) "" + QString(s) + ""
#define QF_SARG(s) "'" + QString(s) + "'"
#define QF_IARG(i) "" + QString::number(i) + ""

#define QF_QUOTE(x) #x
#define QF_EXPAND_AND_QUOTE(x) QF_QUOTE(x)
#define QF_QUOTE_QSTRINGLITERAL(x) QStringLiteral(#x)

#define QF_FIELD_IMPL(ptype, lower_letter, upper_letter, name_rest) \
	private: ptype m_##lower_letter##name_rest; \
	public: ptype lower_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: bool set##upper_letter##name_rest(const ptype &val) { \
		if(!(m_##lower_letter##name_rest == val)) { m_##lower_letter##name_rest = val; return true; } \
		return false; \
	}

#define QF_FIELD_IMPL2(ptype, lower_letter, upper_letter, name_rest, default_value) \
	private: ptype m_##lower_letter##name_rest = default_value; \
	public: ptype lower_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: bool set##upper_letter##name_rest(const ptype &val) { \
		if(!(m_##lower_letter##name_rest == val)) { m_##lower_letter##name_rest = val; return true; } \
		return false; \
	}

#define QF_FIELD_BOOL_IMPL2(lower_letter, upper_letter, name_rest, default_value) \
	private: bool m_##lower_letter##name_rest = default_value; \
	public: bool is##upper_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: bool set##upper_letter##name_rest(bool val) { \
		if(!(m_##lower_letter##name_rest == val)) { m_##lower_letter##name_rest = val; return true; } \
		return false; \
	}

#define QF_FIELD_BOOL_IMPL(lower_letter, upper_letter, name_rest) \
	QF_FIELD_BOOL_IMPL2(lower_letter, upper_letter, name_rest, false)

#define QF_PROPERTY_IMPL2(ptype, lower_letter, upper_letter, name_rest, default_value) \
	private: ptype m_##lower_letter##name_rest = default_value; \
	public: Q_SIGNAL void lower_letter##name_rest##Changed(const ptype &new_val); \
	public: ptype lower_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: Q_SLOT bool set##upper_letter##name_rest(const ptype &val) { \
		if(m_##lower_letter##name_rest != val) { \
			m_##lower_letter##name_rest = val; \
			emit lower_letter##name_rest##Changed(m_##lower_letter##name_rest); \
			return true; \
		} \
		return false; \
	}

#define QF_PROPERTY_IMPL(ptype, lower_letter, upper_letter, name_rest) \
	private: ptype m_##lower_letter##name_rest; \
	public: Q_SIGNAL void lower_letter##name_rest##Changed(const ptype &new_val); \
	public: ptype lower_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: Q_SLOT bool set##upper_letter##name_rest(const ptype &val) { \
		if(!(m_##lower_letter##name_rest == val)) { \
			m_##lower_letter##name_rest = val; \
			emit lower_letter##name_rest##Changed(m_##lower_letter##name_rest); \
			return true; \
		} \
		return false; \
	}

#define QF_PROPERTY(ptype, lower_letter, upper_letter, name_rest) \
	Q_PROPERTY(ptype lower_letter##name_rest READ lower_letter##name_rest WRITE set##upper_letter##name_rest NOTIFY lower_letter##name_rest##Changed) \
	QF_PROPERTY_IMPL(ptype, lower_letter, upper_letter, name_rest)

#define QF_PROPERTY2(ptype, lower_letter, upper_letter, name_rest) \
	Q_PROPERTY(ptype lower_letter##name_rest READ lower_letter##name_rest WRITE set##upper_letter##name_rest NOTIFY lower_letter##name_rest##Changed) \
	QF_PROPERTY_IMPL2(ptype, lower_letter, upper_letter, name_rest, default_value)

#define QF_PROPERTY_OBJECT_IMPL(ptype, lower_letter, upper_letter, name_rest) \
	private: ptype m_##lower_letter##name_rest = nullptr; \
	public: Q_SIGNAL void lower_letter##name_rest##Changed(ptype new_val); \
	public: ptype lower_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: Q_SLOT bool set##upper_letter##name_rest(ptype val) { \
		if(m_##lower_letter##name_rest != val) { \
			m_##lower_letter##name_rest = val; \
			if(m_##lower_letter##name_rest != nullptr) \
				if(m_##lower_letter##name_rest->parent() == nullptr) { \
					/*qfWarning() << "setting object without parent:" << m_##lower_letter##name_rest << "to property:" << QF_QUOTEME(lower_letter##name_rest) << "of:" << this; \
					qfWarning() << "reparenting:" << m_##lower_letter##name_rest << "to:" << this; \
					m_##lower_letter##name_rest->setParent(this); */\
				} \
			emit lower_letter##name_rest##Changed(m_##lower_letter##name_rest); \
			return true; \
		}\
		return false; \
	}

#define QF_PROPERTY_BOOL_IMPL2(lower_letter, upper_letter, name_rest, default_value) \
	private: bool m_##lower_letter##name_rest = default_value; \
	public: Q_SIGNAL void lower_letter##name_rest##Changed(const bool &new_val); \
	public: bool is##upper_letter##name_rest() const {return m_##lower_letter##name_rest;} \
	public: Q_SLOT bool set##upper_letter##name_rest(bool val) { \
		if(m_##lower_letter##name_rest != val) { \
			m_##lower_letter##name_rest = val; \
			emit lower_letter##name_rest##Changed(m_##lower_letter##name_rest); \
			return true; \
		}\
		return false; \
	}

#define QF_PROPERTY_BOOL_IMPL(lower_letter, upper_letter, name_rest) \
	QF_PROPERTY_BOOL_IMPL2(lower_letter, upper_letter, name_rest, false)

#define QF_PROPERTY_BOOL(lower_letter, upper_letter, name_rest) \
	Q_PROPERTY(bool lower_letter##name_rest READ is##upper_letter##name_rest WRITE set##upper_letter##name_rest NOTIFY lower_letter##name_rest##Changed) \
	QF_PROPERTY_BOOL_IMPL(lower_letter, upper_letter, name_rest)

#define QF_PROPERTY_BOOL2(lower_letter, upper_letter, name_rest, default_value) \
	Q_PROPERTY(bool lower_letter##name_rest READ is##upper_letter##name_rest WRITE set##upper_letter##name_rest NOTIFY lower_letter##name_rest##Changed) \
	QF_PROPERTY_BOOL_IMPL2(lower_letter, upper_letter, name_rest, default_value)

#define QF_VARIANTMAP_FIELD(ptype, getter_prefix, setter_prefix, name_rest) \
	public: bool getter_prefix##name_rest##_isset() const {return contains(QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest));} \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(value(QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest)));} \
	public: void setter_prefix##name_rest(const ptype &val) {(*this)[QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest)] = QVariant::fromValue(val);}
/// for default values other than QVariant()
#define QF_VARIANTMAP_FIELD2(ptype, getter_prefix, setter_prefix, name_rest, default_value) \
	public: bool getter_prefix##name_rest##_isset() const {return contains(QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest));} \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(value(QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest), default_value));} \
	public: void setter_prefix##name_rest(const ptype &val) {(*this)[QF_QUOTE_QSTRINGLITERAL(getter_prefix##name_rest)] = QVariant::fromValue(val);}
	//since c++14 public: auto& setter_prefix##name_rest(const ptype &val) {(*this)[QF_QUOTEME(getter_prefix##name_rest)] = val; return *this;}

/// for implicitly shared classes properties
#define QF_SHARED_CLASS_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
	public: const ptype& getter_prefix##name_rest() const {return d->getter_prefix##name_rest;} \
	public: void setter_prefix##name_rest(const ptype &val) {d->getter_prefix##name_rest = val;}

/// for implicitly shared classes properties
#define QF_SHARED_CLASS_BIT_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
	public: ptype getter_prefix##name_rest() const {return d->getter_prefix##name_rest;} \
	public: void setter_prefix##name_rest(const ptype &val) {d->getter_prefix##name_rest = val;}

class QString;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Utils
{
public:
	static const QString &nullValueString();
	static void parseFieldName(const QString& full_field_name, QString *pfield_name = nullptr, QString *ptable_name = nullptr, QString *pdb_name = nullptr);
	static QString composeFieldName(const QString &field_name, const QString &table_name = QString(), const QString &db_name = QString());
	/// @returns: True if @a field_name1 ends with @a field_name2. Comparision is case insensitive
	static bool fieldNameEndsWith(const QString &field_name1, const QString &field_name2);
	static bool fieldNameCmp(const QString &fld_name1, const QString &fld_name2);
	static QVariant retypeVariant(const QVariant &_val, int meta_type_id);
	static QVariant retypeStringValue(const QString &str_val, const QString &type_name);

	static int findCaption(const QString &caption_format, int from_ix, QString *caption);
	/**
	 * @brief findCaptions
	 * Finds in string all captions in form {{captionName}}
	 * @param str
	 * @return Set of found captions.
	 */
	static QSet<QString> findCaptions(const QString &caption_format);
	static QString replaceCaptions(const QString format_str, const QString &caption_name, const QVariant &caption_value);
	static QString replaceCaptions(const QString format_str, const QVariantMap &replacements);

	static QString removeJsonComments(const QString &json_str);

	static int versionStringToInt(const QString &version_string);
	static QString intToVersionString(int ver);

	/// invoke method of prototype bool method()
	static bool invokeMethod_B_V(QObject *obj, const char *method_name);

	template <class T>
	static T findParent(const QObject *_o, bool throw_exc = qf::core::Exception::Throw)
	{
		T t = nullptr;
		QObject *o = const_cast<QObject*>(_o);
		while(o) {
			o = o->parent();
			if(!o)
				break;
			t = qobject_cast<T>(o);
			if(t)
				break;
		}
		if(!t && throw_exc) {
			QF_EXCEPTION(QString("object 0x%1 has not any parent of requested type.").arg(reinterpret_cast<uintptr_t>(_o), 0, 16));
		}
		return t;
	}

	template <typename V, typename... T>
	constexpr static inline auto make_array(T&&... t) -> std::array < V, sizeof...(T) >
	{
		return {{ std::forward<T>(t)... }};
	}

	static QStringList parseProgramAndArgumentsList(const QString &command_line);
};

}}

#endif
