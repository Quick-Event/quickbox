#ifndef QF_CORE_UTILS_SVALUE_H
#define QF_CORE_UTILS_SVALUE_H

#include "../core/coreglobal.h"

#include <QSharedData>
#include <QVariant>
#include <QJsonDocument>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT SValue
{
public:
	static const QString FLD_VALUE;
	static const QString FLD_META;
public:
	SValue();
	SValue(const QVariant &v);
	virtual ~SValue();
private:
	struct Data : QSharedData {
		QVariant value;
	};
	QExplicitlySharedDataPointer<Data> d;
protected:
	//QString dataHex() const;
	SValue property_helper(const QString &name);
	SValue property_helper(int ix);
public:
	bool isValid() const {return d->value.isValid();}
	QVariant::Type valueType() const {return d->value.type();}
	bool isList() const {return valueType() == QVariant::List;}
	bool isMap() const {return valueType() == QVariant::Map;}
	/// pokud je to objekt, a obsahuje property \a name vraci true a do val da jeji hodnotu
	bool hasProperty(const QString &name, QVariant *val = NULL) const;
	/// pokud je to objekt, vraci jmena jeho properties
	QStringList keys() const;
	/// pokud je to pole, vraci pocet prvku
	int count() const {return isList()? d->value.toList().count(): 0;}
	QVariant value() const;
	void setValue(const QVariant &val);
	void setValue(const SValue &sval);
	QVariant property(const QString &name, const QVariant &default_value = QVariant()) const;
	QVariant propertyOnPath(const QStringList &path, const QVariant &default_value = QVariant()) const;
	QVariant propertyOnPath(const QString &path, const QVariant &default_value = QVariant()) const {return propertyOnPath(path.split('.'), default_value);}
	/// pokud je vracena property typu mapa a ta obsahuje field value, bere se on
	QVariant propertyValueOnPath(const QStringList &path, const QVariant &default_value = QVariant()) const;
	QVariant propertyValueOnPath(const QString &path, const QVariant &default_value = QVariant()) const {return propertyValueOnPath(path.split('.'), default_value);}
	/// abych mohl psat neco jako sv.property2("meta").property2("name")
	SValue property2(const QString &name) const {return SValue(property(name));}
	SValue property2(int ix) const {return SValue(property(ix));}
	void setProperty(const QString &name, const QVariant &val);
	void setProperty(const QString &name, const SValue &val) {QVariant v; v.setValue(val); setProperty(name, v);}
	QVariant property(int ix, const QVariant &default_value = QVariant()) const;
	/// pokud je val primitivni typ je to v pohode
	/// pokud je val SValue tak taky
	/// pokud je val QVariantList nebo QVariantMap, prevede se nejprve na SValue
	void setProperty(int ix, const QVariant &val);
	void setProperty(int ix, const SValue &val) {QVariant v; v.setValue(val); setProperty(ix, v);}
	void setPropertyOnPath(const QStringList &path, const QVariant &val) const;
	void setPropertyOnPath(const QString &path, const QVariant &val) const {return setPropertyOnPath(path.split('.'), val);}

	/// Do hodnoty vracene timto operatorem lze diky explicitnimu sdileni priradit.
	/// Pokud navraceny objekt referencuje nejake misto ve stromu, je treba mit na pozoru, co se pri prirazeni skutecne deje.
	/// Strom je modifikovan pouze v pripade, ze je prirazen QVariant jineho typu nez SValue
	/// pokud se prirazuje neco jineho nez SValue operator= se chova jako setValue()
	/// pokud se prirazuje SValue operator= se chova jako pretoceni dat na data svalue na prave strane prirazeni
	/**
		\code
		SValue sv;
		sv["rows"][10] = 1; /// dojde k modifikaci sv

		SValue sv3;
		QVariantList vl;
		vl << 1 << 2 << 3;
		sv3["rows"] = vl;  /// dojde k modifikaci sv

		sv["rows"][10] = sv3 /// strom zustane nezmenen, protoze sv["rows"][10] vrati SValue, ktera referencuje sv["rows"][10], ale pri prirazeni se tato reference pretoci na data sv3
		sv["rows"].setProperty(10, sv3) nebo sv["rows"][10].setValue(sv3)  /// toto je OK

		SValue sv4 = sv["rows"][10]; // vznikne promenna sv4, ktera referencuje svalue obsahujici hodnotu 1, ta rovnez nahradi primitivni hodnotu 1, ktera byla v listu dosud. viz. property_helper()
		// z nasl. radku odkomentujte pouze 1
		//sv["rows"][10] = sv3; // nestane se nic, vznikne svalue obsahujici hodnotu 1, ktera nahradi primitivni hodnotu 1, ktera byla v listu dosud. viz. property_helper()
		sv["rows"].setProperty(10, sv3); // sv se zmeni, sv["rows"][10] nyni referencuje stejna data, jako sv3
		//sv4 = sv3; // nestane se nic, sv4 ted proste referencuje data, ktera refrencuje sv3
		//sv4 = sv3.value(); // dojde k prirazeni do sv["rows"][10], sv se zmeni
		//sv4 = 3; // dojde k prirazeni do sv["rows"][10], sv se zmeni
		\endcode
		\code
		SValue sv;
		SValue sv3;
		QVariantList vl;
		vl << 1 << 2 << 3;
		// Bacha na to
		sv[10] = sv3; // nestane se nic, viz. priklady nahore. sv[10] vraci svalue referencujici 10ty prvek listu, prirazenim se stane pouze to, ze vracena svalue bude referencovat stejna data jako sv3
		sv[10] = 3; // to je OK, dojde k prirazeni 3 to sv[10] (k prirazeni dojde vzdy pokud na prave strane neni QFScriptValue)
		sv.setProperty(10, sv3); // toto je treba pouzit pro prirazeni SValue do sv[10]
		\endcode
		*/
	SValue operator[](const QString &name) {return property_helper(name);}
	SValue operator[](int ix) {return property_helper(ix);}
	SValue& operator=(const QVariant &v) {setValue(v); return *this;}
	///SValue& operator=(const SValue &sv); tento operator je implicitni, neni treba ho psat
	SValue& operator+=(const QVariantMap &m);
	bool operator==(const SValue &other) const {return d == other.d;}
	bool operator!=(const SValue &other) const {return d != other.d;}
public:
	/// convert SValue to nested variants maps
	QVariant toVariant() const;
	/// wrap SValue to QVariant
	QVariant variantCast() const;

	/// rozparsuje JSON a vyrobi rekurzivne cely strom
	void setJson(const QByteArray &json);
	void setJson(const QVariant &json) {setVariant(json);}
	void setVariant(const QVariant &json);

	// data can contain QJSValues from QML scripts, convert them to pure c++ QVariants (QVariantMap, QVariantList)
	//void removeJSTypes();

	static SValue fromJson(const QVariant &json) {SValue ret; ret.setJson(json); return ret;}

	QByteArray toJson(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
	QByteArray serialize() const;
	QString toString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
};

}}}

Q_DECLARE_METATYPE(qf::core::utils::SValue)

#endif

