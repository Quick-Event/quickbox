#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

namespace qf {
namespace core {

class Settings : public QSettings
{
	Q_OBJECT
private:
	typedef QSettings Super;
public:
	explicit Settings(QObject *parent = 0);
public slots:
	void beginGroup(const QString & prefix) {Super::beginGroup(prefix);}
	int beginReadArray(const QString & prefix) {return Super::beginReadArray(prefix);}
	void beginWriteArray(const QString & prefix, int size = -1) {Super::beginWriteArray(prefix, size);}
	void endArray() {Super::endArray();}
	void endGroup() {Super::endGroup();}
	void setArrayIndex(int i) {Super::setArrayIndex(i);}
	QString group() const {return Super::group();}

	void setValue(const QString & key, const QVariant & value) {Super::setValue(key, value);}
	QVariant value(const QString & key, const QVariant & default_value = QVariant()) const {return Super::value(key, default_value);}

	void sync() {Super::sync();}
};

}}

#endif // SETTINGS_H
