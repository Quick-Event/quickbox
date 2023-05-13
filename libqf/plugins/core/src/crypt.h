#ifndef QF_CORE_QML_CRYPT_H
#define QF_CORE_QML_CRYPT_H

#include <qf/core/utils/crypt.h>

#include <QObject>
//#include <QQmlComponent>

namespace qf {
namespace core {
namespace qml {

class Crypt : public QObject
{
	Q_OBJECT
	//QML_ELEMENT
public:
	explicit Crypt(QObject *parent = nullptr);
public:
	Q_INVOKABLE void initGenerator(unsigned a, unsigned b, unsigned max_rand);
	Q_INVOKABLE QString encrypt(const QString &s, int min_length = 10) const;
	Q_INVOKABLE QString decrypt(const QString &s) const;
private:
	qf::core::utils::Crypt m_crypt;
};

}}}

#endif
