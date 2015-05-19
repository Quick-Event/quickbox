#ifndef QF_CORE_QML_QMLFILESINGLETON_H
#define QF_CORE_QML_QMLFILESINGLETON_H

#include <QObject>

namespace qf {
namespace core {
namespace qml {

class QmlFileSingleton : public QObject
{
	Q_OBJECT
public:
	explicit QmlFileSingleton(QObject *parent = 0);
	~QmlFileSingleton();

signals:

public slots:
};

} // namespace qml
} // namespace core
} // namespace qf

#endif // QF_CORE_QML_QMLFILESINGLETON_H
