#ifndef QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTOPTIONS_H
#define QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTOPTIONS_H

#include "../qmlwidgetsglobal.h"

#include <QString>

class QObject;
class QVariant;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT IPersistentOptions
{
public:
	IPersistentOptions(QObject *controlled_object);
public:
	virtual QString persistentOptionsPath();
	void setPersistentOptionsPath(const QString &path);

	virtual QVariant persistentOptions() = 0;
	virtual void setPersistentOptions(const QVariant &opts) = 0;

	QObject* controlledObject() {return m_controlledObject;}
private:
	QObject *m_controlledObject;
	QString m_path;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTOPTIONS_H
