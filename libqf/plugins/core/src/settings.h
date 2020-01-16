#ifndef QF_CORE_QML_SETTINGS_H
#define QF_CORE_QML_SETTINGS_H

#include <qf/core/utils/settings.h>

namespace qf {
namespace core {
namespace qml {

class Settings : public qf::core::utils::Settings
{
	Q_OBJECT
private:
	typedef qf::core::utils::Settings Super;
public:
	explicit Settings(QObject *parent = nullptr);
	~Settings() Q_DECL_OVERRIDE;
public slots:
	void beginGroup(const QString & prefix) {Super::beginGroup(prefix);}
	int beginReadArray(const QString & prefix) {return Super::beginReadArray(prefix);}
	void beginWriteArray(const QString & prefix, int size = -1) {Super::beginWriteArray(prefix, size);}
	void endArray() {Super::endArray();}
	void endGroup() {Super::endGroup();}
	void setArrayIndex(int i) {Super::setArrayIndex(i);}
	QString group() const {return Super::group();}

	void sync() {Super::sync();}
};

}}}

#endif
