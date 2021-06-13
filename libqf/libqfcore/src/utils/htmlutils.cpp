#include "htmlutils.h"
#include "../core/assert.h"

using namespace qf::core::utils;

HtmlUtils::FromHtmlListOptions::FromHtmlListOptions()
{
	static FromHtmlListOptions default_options = FromHtmlListOptions(0);
	if(default_options.isEmpty()) {
		default_options.setEncoding(QStringLiteral("utf-8"));
		default_options.setDocumentTitle(QStringLiteral("html document"));
		default_options.setStyle(QStringLiteral(
									 "		<style type=\"text/css\">\n"
									 "			body {font-family: sans-serif}\n"
									 "			h1, h2, h3, h4, h5 {\n"
									 "				color: #2d2d2d;\n"
									 "				//font-family: sans-serif;\n"
									 "				//font-size: 100%;\n"
									 "				//font-weight: normal;\n"
									 "				clear: left;\n"
									 "			}\n"
									 "			h1 {font-size: 220%; font-weight: bold;}\n"
									 "			h2 {font-size: 150%; font-weight: bold; border-bottom: 1px  dotted #666 ;}\n"
									 "			h3 {font-size: 120%;  }\n"
									 "			.text {text-align:left}\n"
									 "			.code {background-color: lightgray; text-align:left}\n"
									 "\n"
									 "			.nowrap { white-space: nowrap; }\n"
									 "			.right { text-align: right; }\n"
									 "			.red { color: red; }\n"
									 "			.blue { color: blue; }\n"
									 "			.green { color: green; }\n"
									 "			.bold { font-weight: bold; }\n"
									 "			.bt { border-top: 1px solid gray; }\n"
									 "			.bb { border-bottom: 1px solid gray; }\n"
									 "			.bl { border-left: 1px solid gray; }\n"
									 "			.br { border-right: 1px solid gray; }\n"
									 "			.btb { border-top: 2px solid black; }\n"
									 "			.bbb { border-bottom: 2px solid black; }\n"
									 "			.blb { border-left: 2px solid black; }\n"
									 "			.brb { border-right: 2px solid black; }\n"
									 "			table { border: 0px solid gray; border-spacing: 0px 0px;}\n"
									 "			tr.odd { background-color: #e6e6e6; }\n"
									 "			th { padding: 3px; border: 0px; background-color: #8dd35f; }\n"
									 "			td { padding: 3px; border: 0px; }\n"
									 "		</style>"
									 ));
	}
	*this = default_options;
}

HtmlUtils::FromHtmlListOptions::FromHtmlListOptions(const QVariantMap &o)
	: FromHtmlListOptions()
{
	QMapIterator<QString, QVariant> it(o);
	while(it.hasNext()) {
		it.next();
		this->operator [](it.key()) = it.value();
	}
}

HtmlUtils::FromXmlListOptions::FromXmlListOptions()
{
	static FromXmlListOptions default_options = FromXmlListOptions(0);
	if(default_options.isEmpty()) {
		default_options.setEncoding(QStringLiteral("utf-8"));
		default_options.setDocumentTitle(QStringLiteral("xml document"));
	}
	*this = default_options;
}

HtmlUtils::FromXmlListOptions::FromXmlListOptions(const QVariantMap &o)
	: FromXmlListOptions()
{
	QMapIterator<QString, QVariant> it(o);
	while(it.hasNext()) {
		it.next();
		this->operator [](it.key()) = it.value();
	}
}

QString HtmlUtils::fromHtmlList(const QVariantList &body_list, const HtmlUtils::FromHtmlListOptions &options)
{
	QString html_header =
		"<?xml version=\"1.0\" encoding=\"{{encoding}}\"?>\n"
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
		"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
		"	<head>\n"
		"		<meta http-equiv=\"content-type\" content=\"text/html; charset={{encoding}}\" />\n"
		"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
		"		<title>{{documentTitle}}</title>\n"
		"		{{style}}\n"
		"	</head>\n";
	QString body = fromHtmlList_helper(body_list, QString(), options);
	QString html_footer = "</html>";
	QString ret = html_header + body + html_footer;
	ret = qf::core::Utils::replaceCaptions(ret, options);
	return ret;
}

QString HtmlUtils::fromXmlList(const QVariantList &body_list, const FromXmlListOptions &options)
{
	QString xml_header =
		"<?xml version=\"1.0\" encoding=\"{{encoding}}\"?>\n";
	if(!options.docType().isEmpty())
		xml_header += "<!DOCTYPE {{docType}}>\n";
	QString body = fromHtmlList_helper(body_list, QString());
	QString ret = xml_header + body;
	ret = qf::core::Utils::replaceCaptions(ret, options);
	return ret;
}

QString HtmlUtils::fromHtmlList_helper(const QVariant &item, const QString &indent, const HtmlUtils::FromHtmlListOptions &options)
{
	QString ret;
	Q_UNUSED(options)
	if(item.type() == QVariant::List) {
		QVariantList lst = item.toList();
		if(lst.isEmpty())
			return ret;
		QString element_name = lst.first().toString();
		//qfInfo() << element_name << lst.first();
		QF_ASSERT(!element_name.isEmpty(), "Bad element name!", return ret);
		QString attrs_str;
		int ix = 1;
		QVariant attrs = lst.value(ix);
		if(attrs.type() == QVariant::Map) {
			QVariantMap m = attrs.toMap();
			QMapIterator<QString, QVariant> it(m);
			while(it.hasNext()) {
				it.next();
				attrs_str += ' ' + it.key() + '=' + '"' + it.value().toString() + '"';
			}
			ix++;
		}
		bool has_children = (ix < lst.count());
		ret += '\n' + indent;
		if(has_children) {
			ret += '<' + element_name + attrs_str + '>';
			QString indent2 = indent + '\t';
			bool has_child_elemet = false;
			for (; ix < lst.count(); ++ix) {
				QVariant v = lst[ix];
				if(!v.toList().isEmpty())
					has_child_elemet = true;
				ret += fromHtmlList_helper(v, indent2, options);
			}
			if(has_child_elemet)
				ret += '\n' + indent;
			ret += "</" + element_name + '>';
		}
		else {
			ret += '<' + element_name + attrs_str + "/>";
		}
	}
	else {
		ret = item.toString();
	}
	return ret;
}

QVariantList HtmlUtils::createHtmlTable(const QString &title, const QStringList &flds, const QVariantList &rows)
{
	QVariantList div = QVariantList() << QStringLiteral("div");
	div.insert(div.length(), QVariantList() << QStringLiteral("h2") << title);
	QVariantList table = QVariantList() << QStringLiteral("table");
	QVariantList header = QVariantList() << QStringLiteral("tr");
	for(auto fld : flds)
		header.insert(header.length(), QVariantList() << QStringLiteral("th") << fld);
	table.insert(table.length(), header);
	table << rows;
	div.insert(div.length(), table);
	return div;
}


