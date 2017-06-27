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
		QF_VARIANTMAP_FIELD2(QString, e, setE, ncoding, "utf8")
		QF_VARIANTMAP_FIELD2(QString, s, setS, tyle, \
								 "		<style type=\"text/css\">\n" \
								 "			body {font-family: Verdana, sans-serif}\n" \
								 "			h1, h2, h3, h4, h5 {\n" \
								 "				color: black;\n" \
								 "				font-family: \"Lucida Grande\", Verdana, Lucida, Helvetica, Arial, sans-serif;\n" \
								 "				font-size: 100%;\n" \
								 "				font-weight: normal;\n" \
								 "				clear: left;\n" \
								 "			}\n" \
								 "			h1 {font-size: 220%; font-weight: bold;}\n" \
								 "			h2 {font-size: 150%; font-weight: bold; border-bottom: 1px  dotted #666 ;}\n" \
								 "			h3 {font-size: 120%;  }\n" \
								 "			.text {text-align:left}\n" \
								 "			.code {background-color: lightgray; text-align:left}\n" \
								 "\n" \
								 "			table { border: 1px solid gray; border-spacing: 0px 0px;}\n" \
								 "			tr.odd { background-color: lavender; }\n" \
								 "			th { padding: 3px; border: 0px solid #53bc1f; background-color: #91e369; }\n" \
								 "			td { padding: 3px; border: 0px solid #53bc1f; }\n" \
								 "		</style>")
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
