#ifndef QF_CORE_NETWORK_NETWORKREPLY_H
#define QF_CORE_NETWORK_NETWORKREPLY_H

#include "../core/coreglobal.h"

#include <QNetworkReply>

namespace qf {
namespace core {
namespace network {

class QFCORE_DECL_EXPORT NetworkReply : public QNetworkReply
{
	Q_OBJECT
public:
	explicit NetworkReply(QObject *parent = 0);
};

}}}

#endif // QF_CORE_NETWORK_NETWORKREPLY_H
