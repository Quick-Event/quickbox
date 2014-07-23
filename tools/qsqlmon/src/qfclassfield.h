#ifndef QFCLASSFIELD_H
#define QFCLASSFIELD_H
/*
#ifndef QF_FIELD_INTERNAL
#define QF_FIELD_INTERNAL(ptype, perm_getter, getter_prefix, perm_setter, setter_prefix, name_rest) \
	protected: ptype f_##getter_prefix##name_rest; \
	perm_getter: ptype getter_prefix##name_rest() const {return f_##getter_prefix##name_rest;} \
	perm_setter: ptype setter_prefix##name_rest() {ptype fp = f_##getter_prefix##name_rest; f_##getter_prefix##name_rest = fp; return fp;}
#endif

#define QF_FIELD_R(ptype, getter_prefix, setter_prefix, name_rest) \
		QF_FIELD_INTERNAL(ptype, public, getter_prefix, protected, setter_prefix, name_rest)
#define QF_FIELD_W(ptype, getter_prefix, setter_prefix, name_rest) \
		QF_FIELD_INTERNAL(ptype, protected, getter_prefix, public, setter_prefix, name_rest)
#define QF_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
		QF_FIELD_INTERNAL(ptype, public, getter_prefix, public, setter_prefix, name_rest)
*/

#define QF_FIELD_R(ptype, getter_prefix, setter_prefix, name_rest) \
	protected: ptype f_##getter_prefix##name_rest; \
	public: ptype getter_prefix##name_rest() const {return f_##getter_prefix##name_rest;} \
	protected: void setter_prefix##name_rest(const ptype &val) {f_##getter_prefix##name_rest = val;}

#define QF_FIELD_W(ptype, getter_prefix, setter_prefix, name_rest) \
	protected: ptype f_##getter_prefix##name_rest; \
	protected: ptype getter_prefix##name_rest() const {return f_##getter_prefix##name_rest;} \
	public: void setter_prefix##name_rest(const ptype &val) {f_##getter_prefix##name_rest = val;}

#define QF_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest) \
	protected: ptype f_##getter_prefix##name_rest; \
	public: ptype getter_prefix##name_rest() const {return f_##getter_prefix##name_rest;} \
	public: void setter_prefix##name_rest(const ptype &val) {f_##getter_prefix##name_rest = val;}
/*
#define QF_QUOTEME_CLASSFIELD(x) #x

#define QF_MAP_FIELD_R(ptype, getter_prefix, setter_prefix, name_rest, variant_map_fileld) \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(variant_map_fileld.value(QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest)));} \
	protected: void setter_prefix##name_rest(const ptype &val) {variant_map_fileld[QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest)] = val;}

#define QF_MAP_FIELD_RW(ptype, getter_prefix, setter_prefix, name_rest, variant_map_fileld) \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(variant_map_fileld.value(QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest)));} \
	public: void setter_prefix##name_rest(const ptype &val) {variant_map_fileld[QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest)] = val;}
/// pokud existuje default value jina nez QVariant
#define QF_MAP_FIELD2_RW(ptype, getter_prefix, setter_prefix, name_rest, variant_map_fileld, default_value) \
	public: ptype getter_prefix##name_rest() const {return qvariant_cast<ptype>(variant_map_fileld.value(QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest), default_value));} \
	public: void setter_prefix##name_rest(const ptype &val) {variant_map_fileld[QF_QUOTEME_CLASSFIELD(getter_prefix##name_rest)] = val;}
*/
#endif // QFCLASSFIELD_H
