#include "settings.h"

#include "../core/log.h"
#include "../core/string.h"

using namespace qf::core::utils;

void Settings::setValue(const QString &key, const QVariant &value)
{
	QVariant v = value;
	if(v.type() == QVariant::String)
		v = '"' + v.toString() + '"';
	Super::setValue(key, v);
}

QVariant Settings::value(const QString &key, const QVariant &default_value) const
{
	QVariant v = Super::value(key, QVariant());
	if(!v.isValid())
		return default_value;
	QVariant ret = v;
	if(v.type() == QVariant::String) {
		String s = v.toString();
		if(s.value(0) == '"' && s.value(-1) == '"') {
			ret = s.slice(1, -1);
		}
		else {
			if(s == "true" || s == "false") {
				ret = v.toBool();
			}
			else {
				bool ok;
				ret = v.toInt(&ok);
				if(!ok) {
					ret = v.toDouble(&ok);
					if(!ok) {
						qfError() << "Cannot gues type of value in setting key:" << key << "val:" << v;
						ret = v;
					}
				}
			}
		}
	}
	return ret;
}

