#ifndef QF_CORE_UTILS_HTMLUTILS_H
#define QF_CORE_UTILS_HTMLUTILS_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

#include <QVariantMap>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT HtmlUtils
{
public:
	class QFCORE_DECL_EXPORT FromHtmlListOptions : public QVariantMap
	{
		QF_VARIANTMAP_FIELD2(QString, d, setD, ocumentTitle, QStringLiteral("html document"))
		QF_VARIANTMAP_FIELD(QString, s, setS, tyle)
		QF_VARIANTMAP_FIELD2(QString, e, setE, ncoding, "utf-8")

		FromHtmlListOptions(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}
	};
	static QString fromHtmlList(const QVariantList &body_list, const FromHtmlListOptions &options = FromHtmlListOptions());
private:
	static QString fromHtmlList_helper(const QVariant &item, const QString &indent, const FromHtmlListOptions &options);
};

} // namespace utils
} // namespace core
} // namespace qf

#endif // QF_CORE_UTILS_HTMLUTILS_H
