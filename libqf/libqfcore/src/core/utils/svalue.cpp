#include "svalue.h"

#include "../log.h"

#include <QJsonDocument>

using namespace qf::core::utils;

//=================================================
//             SValue
//=================================================
const QString SValue::FLD_VALUE = "value";
const QString SValue::FLD_META = "meta";

SValue::SValue()
{
	d = new Data();
}

SValue::SValue(const QVariant &v)
{
	if(v.userType() == qMetaTypeId<SValue>()) {
		*this = qvariant_cast<SValue>(v);
	}
	else {
		d = new Data();
		setValue(v);
	}
}

SValue::~SValue()
{
}

static QVariant value_to_variant(const QVariant &v)
{
	//qfLogFuncFrame() << v.typeName();// << "path:" << path.join("/");
	QVariant ret;
	if(v.userType() == qMetaTypeId<SValue>()) {
		SValue sv = qvariant_cast<SValue>(v);
		ret = value_to_variant(sv.value());
	}
	else if(v.type() == QVariant::List) {
		/// ARRAY
		QVariantList vl = v.toList();
		//qfInfo() << "vl.count():" << vl.count();
		for(int i=0; i<vl.count(); i++) {
			vl[i] = value_to_variant(vl[i]);
		}
		ret = vl;
	}
	else if(v.type() == QVariant::Map) {
		QVariantMap m = v.toMap();
		QMutableMapIterator<QString, QVariant> i(m);
		while(i.hasNext()) {
			i.next();
			i.setValue(value_to_variant(i.value()));
		}
		ret = m;
	}
	else ret = v;
	return ret;
}

static QVariant variant_to_value(const QVariant &json)
{
	//qfLogFuncFrame() << QFJson::variantToString(json);
	QVariant ret;
	if(json.type() == QVariant::List) {
		/// ARRAY
		SValue sv;
		QVariantList vl = json.toList();
		for(int i=0; i<vl.count(); i++) {
			sv.setProperty(i, variant_to_value(vl[i]));
		}
		ret.setValue(sv);
	}
	else if(json.type() == QVariant::Map) {
		SValue sv;
		QVariantMap m = json.toMap();
		QMapIterator<QString, QVariant> i(m);
		while(i.hasNext()) {
			i.next();
			sv.setProperty(i.key(), variant_to_value(i.value()));
		}
		ret.setValue(sv);
	}
	else ret = json;
	return ret;
}

QVariant SValue::value() const
{
	return d->value;
}

void SValue::setValue(const QVariant &val)
{
	QVariant v = val;
	if(val.userType() == qMetaTypeId<SValue>()) {
		SValue sv = qvariant_cast<SValue>(val);
		v = sv.value();
	}
	d->value = v;
}

void SValue::setValue(const SValue& sval)
{
	d->value = sval.value();
}

bool SValue::hasProperty(const QString &name, QVariant *p_val) const
{
	qfLogFuncFrame();
	bool ret = d->value.toMap().contains(name);
	if(p_val) *p_val = d->value.toMap().value(name);
	qfDebug() << "\t return:" << ret;
	return ret;
}

QStringList SValue::keys() const
{
	qfLogFuncFrame();
	QStringList ret = d->value.toMap().keys();
	qfDebug() << "\t return:" << ret.join(",");
	return ret;
}

QVariant SValue::property(const QString &name, const QVariant &default_value) const
{
	//qfLogFuncFrame() << name << QFJson::variantToString(d->value);
	QVariant ret = d->value.toMap().value(name, default_value);
	//qfDebug() << "\t return:" << ret.toString();
	return ret;
}

QVariant SValue::property(int ix, const QVariant &default_value) const
{
	QVariant ret = d->value.toList().value(ix, default_value);
	return ret;
}

SValue SValue::property_helper(const QString &name)
{
	qfLogFuncFrame() << name;
	SValue ret;
	QVariant v = d->value.toMap().value(name);
	if(v.userType() == qMetaTypeId<SValue>()) {
		/// je to v pohode vracim object, ktery jiz existuje
		ret = qvariant_cast<SValue>(v);
	}
	else {
		if(v.isValid()) {
			/// objekt existuje, ale neni SValue, tak ji z nej vyrob
			ret.setValue(v);
		}
		else {
			/// objekt neexistuje
		}
		setProperty(name, ret);
	}
	return ret;
}

SValue SValue::property_helper(int ix)
{
	SValue ret;
	QVariant v = d->value.toList().value(ix);
	if(v.userType() == qMetaTypeId<SValue>()) {
		/// je to v pohode vracim object, ktery jiz existuje
		ret = qvariant_cast<SValue>(v);
	}
	else {
		if(v.isValid()) {
			/// objekt existuje, ale pro podminku create se vraci SValue()
			ret.setValue(v);
		}
		else {
			/// objekt neexistuje
		}
		setProperty(ix, ret);
	}
	return ret;
}

QVariant SValue::propertyOnPath(const QStringList& path, const QVariant& default_value) const
{
	QVariant ret;
	SValue sv = *this;
	for(int ix=0; ix<path.count(); ix++) {
		if(ix == path.count() - 1) {
			ret = sv.property(path.value(ix), default_value);
			break;
		}
		else {
			sv = sv.property2(path.value(ix));
		}
	}
	return ret;
}

QVariant SValue::propertyValueOnPath(const QStringList& path, const QVariant& default_value) const
{
	QVariant ret = propertyOnPath(path, default_value);
	if(ret.userType() == qMetaTypeId<SValue>()) {
		SValue sv = ret.value<SValue>();
		if(sv.hasProperty(FLD_VALUE))
			ret = sv.property(FLD_VALUE);
	}
	return ret;
}

void SValue::setPropertyOnPath(const QStringList& path, const QVariant& val) const
{
	SValue sv = *this;
	for(int ix=0; ix<path.count(); ix++) {
		if(ix == path.count() - 1) {
			sv.setProperty(path.value(ix), val);
			break;
		}
		else {
			QString skey = path.value(ix);
			bool ok;
			int ikey = skey.toInt(&ok);
			if(ok)
				sv = sv.property_helper(ikey);
			else
				sv = sv.property_helper(skey);
		}
	}
}

void SValue::setProperty(const QString &name, const QVariant &val)
{
	QVariantMap m = d->value.toMap();
	if(d->value.isValid() && d->value.type() != QVariant::Map) {
		/// pokud this obsahuje neco, co neni mapa (property map), uloz to do klice "value";
		m["value"] = d->value;
		//qfWarning().noSpace() << "SValue::setProperty(\"" << name << "\", " << val.toString() << ") - Value type changed from " << d->value.typeName() << " to Map";
		//qfInfo() << QFLog::stackTrace();
	}
	QVariant v = val;
	if(val.userType() != qMetaTypeId<SValue>())
		v = variant_to_value(v);
	m[name] = v;
	d->value = m;
}

void SValue::setProperty(int ix, const QVariant &val)
{
	qfLogFuncFrame() << "ix:" << ix << "val:" << val.toString();
	if(d->value.isValid() && d->value.type() != QVariant::List) {
		qfWarning().nospace() << "SValue::setProperty(" << ix << ", " << val.toString() << ") - Value type changed from " << d->value.typeName() << " to List";
	}
	QVariantList l = d->value.toList();
	for(int i=l.count(); i<ix+1; i++) l << QVariant();
	QVariant v = val;
	if(v.userType() == qMetaTypeId<SValue>()) {
		//qfDebug() << "\t already SValue" << QFJson::variantToString(qvariant_cast<SValue>(v).valueToVariant());
	}
	else v = variant_to_value(v);
	l[ix] = v;
	d->value = l;
}


QString SValue::dataHex() const
{
	return QString::number((long)d.data(), 16);
}

QVariant SValue::variantCast() const
{
	QVariant ret;
	ret.setValue(*this);
	return ret;
}

QVariant SValue::toVariant() const
{
	QVariant ret = value_to_variant(variantCast());
	return ret;
}

void SValue::setJson(const QByteArray &json)
{
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(json, &err);
	QVariant v;
	if(err.error != QJsonParseError::NoError)
		qfError() << "SValue::setValueJson ERROR\n" << err.errorString();
	else
		v = doc.toVariant();
	setVariant(v);
}

void SValue::setVariant(const QVariant &json)
{
	qfLogFuncFrame();// << QFJson::variantToString(json);
	QVariant v = variant_to_value(json);
	if(v.userType() == qMetaTypeId<SValue>()) {
		*this = qvariant_cast<SValue>(v);
	}
	else setValue(v);
}

SValue& SValue::operator+=(const QVariantMap &m)
{
	QMapIterator<QString, QVariant> it(m);
	while(it.hasNext()) {
		it.next();
		setProperty(it.key(), it.value());
	}
	return *this;
}

QByteArray SValue::toJson(int indent) const
{
	QJsonDocument doc = QJsonDocument::fromVariant(toVariant());
	QByteArray ba = doc.toJson((indent>=0)? QJsonDocument::Indented: QJsonDocument::Compact);
	return ba;
}

QByteArray SValue::serialize() const
{
	QByteArray ret = toJson(-1);
	return ret;
}
