#ifndef QF_CORE_UTILS_SETTINGS_H
#define QF_CORE_UTILS_SETTINGS_H

#include "../core/coreglobal.h"

#include <QSettings>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT Settings : public QSettings
{
	Q_OBJECT
	typedef QSettings Super;
public:
	Settings(QObject *parent = nullptr) : Super(parent) {}

	void setValue(const QString & key, const QVariant & value);
	QVariant value(const QString & key, const QVariant & default_value = QVariant()) const;

};

}}}

#endif // QF_CORE_UTILS_SETTINGS_H
