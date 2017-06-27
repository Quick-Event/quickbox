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
		QF_VARIANTMAP_FIELD(QString, d, setD, ocumentTitle)
		QF_VARIANTMAP_FIELD(QString, e, setE, ncoding)
		QF_VARIANTMAP_FIELD(QString, s, setS, tyle)
	private:
		FromHtmlListOptions(int) {}
	public:
		FromHtmlListOptions();
		FromHtmlListOptions(const QVariantMap &o);
	};
	class QFCORE_DECL_EXPORT FromXmlListOptions : public QVariantMap
	{
		QF_VARIANTMAP_FIELD(QString, d, setD, ocumentTitle)
		QF_VARIANTMAP_FIELD(QString, d, setD, ocType)
		QF_VARIANTMAP_FIELD(QString, e, setE, ncoding)

	private:
		FromXmlListOptions(int) {}
	public:
		FromXmlListOptions();
		FromXmlListOptions(const QVariantMap &o);
	};
	static QString fromHtmlList(const QVariantList &body_list, const FromHtmlListOptions &options = FromHtmlListOptions());
	static QString fromXmlList(const QVariantList &body_list, const FromXmlListOptions &options = FromXmlListOptions());
private:
	static QString fromHtmlList_helper(const QVariant &item, const QString &indent, const FromHtmlListOptions &options = FromHtmlListOptions());
};

} // namespace utils
} // namespace core
} // namespace qf

#endif // QF_CORE_UTILS_HTMLUTILS_H
