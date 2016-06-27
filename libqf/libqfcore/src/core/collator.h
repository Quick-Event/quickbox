#ifndef QF_CORE_COLLATOR_H
#define QF_CORE_COLLATOR_H

#include "coreglobal.h"
#include "utils.h"

#include <Qt>
#include <QHash>
#include <QLocale>
#include <QSharedDataPointer>

class QStringRef;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Collator
{
public:
	//Collator();
	Collator(QLocale::Language lang);
private:
	class SharedDummyHelper {};
	class Data : public QSharedData
	{
	public:
		unsigned caseSensitivity:1, ignorePunctuation:1;
		QLocale::Language language = QLocale::Czech;
		Data(Qt::CaseSensitivity cs = Qt::CaseInsensitive, bool ip = false)
			: caseSensitivity(cs == Qt::CaseSensitive), ignorePunctuation(ip) {}
	};
	QSharedDataPointer<Data> d;

	Collator(SharedDummyHelper); /// null row constructor
	static const Collator& sharedNull();
public:
	Qt::CaseSensitivity caseSensitivity() const {return d->caseSensitivity? Qt::CaseSensitive: Qt::CaseInsensitive;}
	void setCaseSensitivity(Qt::CaseSensitivity cs) {d->caseSensitivity = (cs == Qt::CaseSensitive);}

	void setIgnorePunctuation(bool on) {d->ignorePunctuation = on;}
	bool ignorePunctuation() const {return d->ignorePunctuation;}

	QF_SHARED_CLASS_FIELD_RW(QLocale::Language, l, setL, anguage)

	int compare(const QString &s1, const QString &s2) const;
	int compare(const QStringRef &s1, const QStringRef &s2) const;

	static QByteArray toAscii7(QLocale::Language lang, const QString &s, bool to_lower);
	static QChar removePunctuation(QLocale::Language language, QChar c);
private:
	int sortIndex(QChar c) const;
	static QHash<QChar, int> sortCache();
};

}}

#endif // QF_CORE_COLLATOR_H
