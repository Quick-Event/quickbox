//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#include "reportpainter.h"
#include "reportprocessor.h"
#include "reportitemband.h"
#include "reportitemreport.h"
#include "style/compiledtextstyle.h"
//#include "../../graphics/graph/graph.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>
#include <qf/core/utils/fileutils.h>

#include <QDate>
#include <QStringBuilder>
#include <QUrl>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//==========================================================
//           ReportItem
//==========================================================
const double ReportItem::Epsilon = 1e-10;
const QString ReportItem::INFO_IF_NOT_FOUND_DEFAULT_VALUE = "$INFO";

ReportItem::ReportItem(ReportItem *_parent)
	: Super(_parent) //--, processor(proc), element(el)
{
	m_keepAll = false;
	m_visible = true;
	//QF_ASSERT_EX(processor != nullptr, "Processor can not be NULL.");
	recentlyPrintNotFit = false;
	//--keepAll = qfc::String(element.attribute("keepall")).toBool();
	//if(keepAll) { qfInfo() << "KEEP ALL is true" << element.attribute("keepall"); }
}

ReportItem::~ReportItem()
{
	//qfDebug() << QF_FUNC_NAME << "##################" << element.tagName();
}
/*--
bool ReportItem::childrenSynced()
{
	qfDebug() << QF_FUNC_NAME<< element.tagName() << "children count:" << itemCount();
	bool synced = true;
	int i = 0;
	for(QDomElement el = element.firstChildElement(); !!el; el = el.nextSiblingElement()) {
		qfDebug() << "\t checking:" << el.tagName() << "i:" << i;
		if(!ReportProcessor::isProcessible(el)) continue; /// nezname elementy ignoruj
		qfDebug() << "\t processible";
		if(i >= itemCount()) {
		 	/// vic znamych elementu nez deti => neco pribylo
			synced = false;
			qfDebug() << "\t more elements";
			break;
		}
		if(el == childAt(i)->element) {
			/// stejny element na stejne posici
			i++;
			continue;
		}
		/// doslo k nejaky zmene nebo deti nejsou dosud vytvoreny
		qfDebug() << "\t other element";
		synced = false;
		break;
	}
	if(i != itemCount()) {
		qfDebug() << "\t divny";
		synced = false;
	}
	qfDebug() << "\treturn:" << synced;
	return synced;
}

void ReportItem::deleteChildren()
{
	clearChildren();
}

void ReportItem::syncChildren()
{
	qfDebug() << QF_FUNC_NAME << element.tagName() << "id:" << element.attribute("id");
	deleteChildren();
	for(QDomElement el = element.firstChildElement(); !!el; el = el.nextSiblingElement()) {
		if(el.tagName() == "script") {
			QString code;
			for(QDomNode nd = el.firstChild(); !nd.isNull(); nd = nd.nextSibling()) {
				if(nd.isCDATASection()) {
					code = nd.toCDATASection().data();
				}
			}
			if(!code.isEmpty()) processor()->scriptDriver()->evaluate(code);
			continue;
		}
		if(!ReportProcessor::isProcessible(el)) continue;
		/// vytvor chybejici item
		processor()->createProcessibleItem(el, this);
	}
}

QString ReportItem::elementAttribute(const QString & attr_name, const QString &default_val)
{
	QString ret = element.attribute(attr_name, default_val);
	static QRegExp rx("script:([A-Za-z]\\S*)\\((.*)\\)");
	if(rx.exactMatch(ret)) {
		QF_ASSERT(processor, "Processor is NULL.");
		QString fn = rx.cap(1);
		ret = rx.cap(2);
		QStringList sl = ret.splitAndTrim(',', '\'');
		QVariantList vl;
		foreach(QString s, sl) vl << s;
		ReportProcessorScriptDriver *sd = processor()->scriptDriver();
		if(sd) {
			sd->setCurrentCallContextItem(this);
			QScriptValue sv = processor()->scriptDriver()->call(this, fn, vl);
			ret = sv.toString();
			//qfInfo() << fn << sl.join(",") << "ret:" << ret;
		}
		else {
			ret = "no script driver";
		}
	}
	return ret;
}
--*/
bool ReportItem::isVisible()
{
	bool ret = processor()->isDesignMode() || m_visible;
	return ret;
}
/*--
ReportItemBand* ReportItem::parentBand()
{
	ReportItem *it = this->parent();
	while(it) {
		if(it->toBand()) return it->toBand();
		it = it->parent();
	}
	return NULL;
}
--*/
ReportItemBand* ReportItem::parentBand()
{
	ReportItemBand *ret = qf::core::Utils::findParent<ReportItemBand*>(this, false);
	return ret;
}
/*--
qfu::TreeTable ReportItem::findDataTable(const QString &name)
{
	qfLogFuncFrame();
	qfu::TreeTable ret;
	ReportItemDetail *d = currentDetail();
	qfDebug() << "\tparent:" << parent() << "parent detail:" << d;
	if(d) {
		qfDebug() << "\tdata row is null:" << d->dataRow().isNull();
		if(d->dataRow().isNull() && !processor()->isDesignMode()) qfWarning().nospace() << "'" << name << "' parent detail datarow is NULL";
		ret = d->dataRow().table(name);
		/// pokud ji nenajde a name neni specifikovano, vezmi 1. tabulku
		if(ret.isNull() && name.isEmpty()) ret = d->dataRow().table(0);
		//qfInfo() << "\ttable name:" << name << "is null:" << ret.isNull();
		//qfInfo() << name << ret.element().toString();
	}
	return ret;
}
--*/
ReportItem::PrintResult ReportItem::checkPrintResult(ReportItem::PrintResult res)
{
	PrintResult ret = res;
	//if(res.value == PrintNotFit) {
	//qfWarning().noSpace() << "PrintNotFit element: '" << element.tagName() << "' id: '" << element.attribute("id") << "' recentlyPrintNotFit: " << recentlyPrintNotFit << " keepall: " << keepAll;
	//}
	if(isKeepAll() && recentlyPrintNotFit && res.value == PrintNotFit) {
		//qfWarning().noSpace() << "PrintNeverFit element: '" << element.tagName() << "' id: '" << element.attribute("id") << "'";
		ret.flags |= FlagPrintNeverFit;
	}
	recentlyPrintNotFit = (ret.value == PrintNotFit);
	return ret;
}

ReportProcessor *ReportItem::processor()
{
	ReportProcessor *ret = nullptr;
	QObject *it = this;
	while(it) {
		ReportItemReport *rir = qobject_cast<ReportItemReport*>(it);
		if(rir) {
			ret = rir->reportProcessor();
			break;
		}
		it = it->QObject::parent();
	}
	QF_ASSERT_EX(ret != nullptr, "ReportItem without ReportProcessor");
	return ret;
}

/*--
QVariant ReportItem::concatenateNodeChildrenValues(const QDomNode & nd)
{
	QVariant ret;
	QDomElement el = nd.toElement();
	for(QFDomNode nd1 = el.firstChild(); !!nd1; nd1 = nd1.nextSibling()) {
		QVariant v1 = nodeValue(nd1);
		if(0) {
			QDomElement eel = nd1.toElement();
			qfInfo() << "node value:" << eel.toString() << v1.toString();
		}
		if(!ret.isValid()) ret = v1;
		else ret = ret.toString() + v1.toString();
	}
	return ret;
}

QString ReportItem::nodeText(const QDomNode &nd)
{
	QVariant node_value = nodeValue(nd);
	if(node_value.canConvert<qfu::TreeTable>()) {
		/// jedna se o XML tabulku, ktera je vysledkem SQL dotazu, vezmi z ni pouze 1. hodnotu na 1. radku
		qfu::TreeTable t = node_value.value<qfu::TreeTable>();
		node_value = t.row(0).value(0);
	}
	QString ret;
	if(nd.isElement()) {
		do {
			QDomElement el = nd.toElement();
			QString null_text = el.attribute("nullText");
			if(!node_value.isValid() && !null_text.isEmpty()) {
				ret = null_text;
				break;
			}
			bool hide_zero = el.attribute("hidezero").toBool();
			if(hide_zero) {
				if(node_value.type() == QVariant::Int && node_value.toInt() == 0) {
					break;
				}
				else if(node_value.type() == QVariant::Double && node_value.toDouble() == 0) {
					break;
				}
				else if(node_value.toString() == "0") {
					break;
				}
			}
			QString currency_symbol;
			{
				QString currency = el.attribute("currency");
				if(!currency.isEmpty()) {
					QFCurrencyRates cr = processor()->currencyRates();
					double d = node_value.toDouble();
					node_value = cr.convertSACToCurrency(d, currency);
					currency_symbol = cr.currencySymbol(currency);
				}
			}
			{
				QString lc_domain = el.attribute("lcDomain");
				if(!lc_domain.isEmpty()) {
					QFDataTranslator *dtr = processor()->dataTranslator();
					if(dtr) {
						node_value = dtr->translate(node_value.toString(), lc_domain);
					}
				}
			}
			QString format = el.attribute("format");
			if(format == "check") {
				//qfInfo() << "hide null:" << hide_null << "is valid:" << node_value.isValid() << "is null:" << node_value.isNull() << "val:" << node_value.toString();
				//if(!hide_null && node_value.isNull()) { /// !isValid() tady nefunguje
				//	ret = "--";
				//}
				//else {
					bool b = node_value.toBool();
					QString s = ReportItemMetaPaint::checkReportSubstitution;
					s.replace("${STATE}", (b)? "1": "0");
					ret = s;
				//}
				//qfWarning() << ret;
			}
			else if(node_value.type() == QVariant::Bool) {
				//qfInfo() << "Date format:" << format;
				ret = QString::fromBool(node_value, format);
			}
			else if(node_value.type() == QVariant::Date) {
				//qfInfo() << "Date format:" << format;
				if(format.isNull()) ret = node_value.toDate().toString(Qf::defaultDateFormat());
				else ret = node_value.toDate().toString(format);
			}
			else if(node_value.type() == QVariant::Time) {
				if(format.isNull()) ret = node_value.toString();
				else ret = node_value.toTime().toString(format);
			}
			else if(node_value.type() == QVariant::DateTime) {
				//qfInfo() << v.toString();
				if(format.isNull()) ret = node_value.toDateTime().toString(Qf::defaultDateTimeFormat());
				else ret = node_value.toTime().toString(format);
			}
			else if(node_value.type() == QVariant::Double) {
				if(!format.isNull()) ret = QString::number(node_value.toDouble(), format);
				else ret = node_value.toString();
			}
			else if(node_value.type() == QVariant::Int) {
				if(!format.isNull()) ret = QString::number(node_value.toInt(), format);
				else ret = node_value.toString();
			}
			else if(node_value.type() == QVariant::String) {
				QString str = node_value.toString();
				ret = str;
				if(!str.isEmpty()) {
					if(format.startsWith("enumz/")) {
						QFDbApplication *db_app = QFDbApplication::instance(!Qf::ThrowExc);
						if(db_app && db_app->connection().isOpen()) {
							QString group_name = format.section('/', 1, 1);
							QString caption = format.section('/', 2);
							if(caption.isEmpty()) caption = "${caption}";
							else if(caption[0] == '\'') caption = QString(caption).slice(1, -1);
							QStringList sl = str.split(',');
							QStringList sl_ret;
							foreach(QString enum_id, sl) {
								QFDbEnum enm = qfDbApp()->dbEnum(group_name, enum_id);
								if(enm.isValid()) sl_ret << enm.fillInPlaceholders(caption);
								else sl_ret << QString("NO_ENUM(%1:%2)").arg(group_name).arg(enum_id);
							}
							ret = sl_ret.join(", ");
						}
					}
				}
			}
			else if(node_value.type() == QVariant::Map) {
				QVariantMap m = node_value.toMap();
				QString str = node_value.toString();
				ret = str;
				if(!str.isEmpty()) {
					if(format.startsWith("enumz/")) {
						QFDbApplication *db_app = QFDbApplication::instance(!Qf::ThrowExc);
						if(db_app && db_app->connection().isOpen()) {
							QString group_name = format.section('/', 1, 1);
							QString caption = format.section('/', 2);
							if(caption.isEmpty()) caption = "${caption}";
							else if(caption[0] == '\'') caption = QString(caption).slice(1, -1);
							QFDbEnum enm = qfDbApp()->dbEnum(group_name, str);
							if(enm.isValid()) ret = enm.fillInPlaceholders(caption);
							else ret = QString("NO_ENUM(%1.%2)").arg(group_name).arg(str);
						}
					}
				}
			}
			else {
				ret = node_value.toString();
			}
			if(!currency_symbol.isEmpty()) {
				bool show_currency_symbol = QString(el.attribute("currencySymbolVisible", "true")).toBool();
				//qfInfo() << currency_symbol << el.attribute("currencySymbolVisible") << show_currency_symbol;
				if(show_currency_symbol) ret = ret % ' ' % currency_symbol;
			}
			{
				QString prefix = el.attribute("prefix");
				if(!prefix.isEmpty()) ret = prefix % ret;
			}
			{
				QString suffix = el.attribute("suffix");
				if(!suffix.isEmpty()) ret = ret % suffix;
			}
		} while(false);
	}
	else ret = node_value.toString();
	//qfInfo() << ret;
	return ret;
}

QVariant ReportItem::nodeValue(const QDomNode &nd)
{
	qfDebug().color(QFLog::Cyan) << QF_FUNC_NAME;
	static const QString S_ATTR_DOMAIN = "domain";
	static const QString S_CAST = "cast";
	static const QString S_KEY = "key";
	static const QString S_EL_DATA = "data";
	QVariant ret;
	qfDebug() << "\tnode type:" << nd.nodeType();
	if(nd.isText()) {
		QString s = nd.toText().data();
		//if(s.endsWith("23")) qfInfo().noSpace() << "\t\ttext: '" << s << "'";
		ret = s;
	}
	else if(nd.isElement()) {
		QDomElement el = nd.toElement();
		if(el.tagName() == S_EL_DATA) {
			QString default_value, default_text;
			{
				QDomElement el1 = el.firstChildElement("default");
				default_text = el1.text();
			}
			default_value = el.attribute("defaultValue", ReportItem::INFO_IF_NOT_FOUND_DEFAULT_VALUE);
			QString cast = el.attribute(S_CAST);
			QString data_src = el.attribute("src").trimmed();
			qfDebug().noSpace() << "\t\tdata: '" << data_src << "'";
			QString domain = el.attribute(S_ATTR_DOMAIN, "row");
			QString data_property_key = el.attribute(S_KEY);
			if(processor()->isDesignMode()) {
				QString s = data_src;
				if(!data_property_key.isEmpty()) { s = s%"."%data_property_key; }
				if(domain == "row") ret = QString('['%s%']');
				else ret = QString('{'%s%'}');
			}
			else {
				bool sql_match = el.attribute("sqlmatch", "true").toBool();
				QVariantList params;
				if(domain == "script") {
					for(QDomElement el_param = el.firstChildElement("param"); !!el_param; el_param = el_param.nextSiblingElement("param")) {
						//qfInfo() << concatenateNodeChildrenValues(el_param).toString();
						params << concatenateNodeChildrenValues(el_param);
					}
				}
				else if(domain == "sql" || domain == "scriptcode") {
					QString code;
					for(QDomNode nd = el.firstChildElement("code").firstChild(); !nd.isNull(); nd = nd.nextSibling()) {
						if(nd.isCDATASection()) {
							code = nd.toCDATASection().data();
							break;
						}
					}
					if(!code.isEmpty()) {
						QVariantMap params;
						for(QDomElement el_param = el.firstChildElement("param"); !!el_param; el_param = el_param.nextSiblingElement("param")) {
							QString param_name = el_param.attribute("name");
							if(!param_name.isEmpty()) {
								params[param_name] = concatenateNodeChildrenValues(el_param);
							}
						}
						QMapIterator<QString, QVariant> i(params);
						while(i.hasNext()) {
							i.next();
							code.replace("${" + i.key() + '}', i.value().toString());
						}
						data_src = code;
					}
				}
				QVariant data_value = value(data_src, domain, params, default_value, sql_match);
				//if(data_src == "ukonceniDny") qfWarning() << "\treturn:" << data_value.toString() << QVariant::typeToName(data_value.type()) << data_value.isValid() << data_value.isNull();
				{
					if(!data_property_key.isEmpty() && data_value.type() == QVariant::String) {
						/// pokud je key a data_value je string zacinajici na { a koncici na }, udelej z nej QVariantMap
						QString fs = data_value.toString().trimmed();
						if(fs.value(0) == '{' && fs.value(-1) == '}') {
							data_value = QFJson::stringToVariant(fs);
						}
					}
				}
				if(data_value.userType() == qMetaTypeId<QFSValue>()) {
					QStringList keys = data_property_key.split('.', QString::SkipEmptyParts);
					foreach(QString key, keys) {
						QFSValue sv(data_value);
						data_value = sv.property(key);
					}
					if(data_value.userType() == qMetaTypeId<QFSValue>()) {
						QFSValue sv(data_value);
						data_value = sv.property("value");
					}
					//qfInfo() << data_value.toString();
				}
				else if(data_value.type() == QVariant::Map) {
					//qfInfo() << "MAP";
					QStringList keys = data_property_key.split('.', QString::SkipEmptyParts);
					foreach(QString key, keys) {
						QVariantMap m = data_value.toMap();
						data_value = m.value(key);
						//qfInfo() << key << "->" << QFJson::variantToString(data_value);
					}
					if(data_value.userType() == qMetaTypeId<QVariantMap>()) {
						QVariantMap m = data_value.toMap();
						data_value = m.value("value");
					}
				}
				if(!cast.isEmpty()) {
					if(cast == "double") {
						data_value = Qf::retypeVariant(data_value, QVariant::Double);
					}
					else if(cast == "int") {
						data_value = Qf::retypeVariant(data_value, QVariant::Int);
					}
					else if(cast == "date") {
						//qfInfo() << "casting date" << data_value.toString();
						data_value = Qf::retypeVariant(data_value, QVariant::Date);
						//qfInfo() << "to" << data_value.toString();
					}
					else if(cast == "time") {
						data_value = Qf::retypeVariant(data_value, QVariant::Time);
					}
				}
				//if(data_src == "reportSValues") { qfInfo() << "data_value type:" << data_value.typeName() << "val:" << data_value.toString(); }
				if(!data_value.isValid() || (data_value.type() == QVariant::String && data_value.toString().isEmpty())) ret = default_text;
				else ret = data_value;
			}
		}
		else {
			qfWarning() << "unprocessible element:" << el.tagName();
		}
	}
	return ret;
}
--*/
#if 0
QVariant ReportItem::value(const QString &data_src, const QString & domain, const QVariantList &params, const QVariant &default_value, bool sql_match)
{
	//qfInfo() << "data_src:" << data_src << "domain:" << domain;
	qfLogFuncFrame() << "data_src:" << data_src << "domain:" << domain << "sql_match:" << sql_match;
	static const QString S_DOMAIN_SYSTEM = "system";
	static const QString S_DOMAIN_REPORT = "report";
	static const QString S_DOMAIN_ROW = "row";
	static const QString S_DOMAIN_TABLE = "table";
	static const QString S_DOMAIN_SCRIPT = "script";
	static const QString S_DOMAIN_SCRIPT_CODE = "scriptcode";
	static const QString S_DOMAIN_SQL = "sql";
	static const QString S_SYSTEM_DATE = "date";
	static const QString S_SYSTEM_TIME = "time";
	static const QString S_TABLE_ROWNO = "ROW_NO()";
	QVariant data_value = default_value;
	bool info_if_not_found = (default_value == ReportItem::INFO_IF_NOT_FOUND_DEFAULT_VALUE);
	if(domain == S_DOMAIN_SYSTEM) {
		if(data_src == S_SYSTEM_DATE) {
			data_value = QDate::currentDate();//.toString(date_format);
		}
		else if(data_src == S_SYSTEM_TIME) {
			data_value = QTime::currentTime();
		}
		else if(data_src == "page") {
			data_value = QString::number(processor()->processedPageNo() + 1);
		}
		else if(data_src == "pageCount") {
			data_value = ReportItemMetaPaint::pageCountReportSubstitution; /// takovyhle blby zkratky mam proto, aby to zabralo zhruba stejne mista jako cislo, za ktery se to vymeni
		}
	}
	/*--
	else if(domain == S_DOMAIN_SCRIPT) {
		try {
			data_value = QFScriptDriver::scriptValueToVariant(processor()->scriptDriver()->call(this, data_src, params));
		}
		catch(QFException &e) {
			qfError() << "Report table data load error:" << e.msg();
		}
	}
	else if(domain == S_DOMAIN_SCRIPT_CODE) {
		try {
			ReportProcessorScriptDriver *sd = processor()->scriptDriver();
			if(sd) {
				sd->setCurrentCallContextItem(this);
				data_value = QFScriptDriver::scriptValueToVariant(sd->evaluate(data_src));
			}
		}
		catch(QFException &e) {
			qfError() << "Report table data load error:" << e.msg();
		}
	}
	else if(domain == S_DOMAIN_SQL) {
		//qfInfo() << "element:" << element.toString();
		if(!processor()->isDesignMode()) {
			QString qs = data_src;
			//qfInfo() << "qs:" << qs;
			if(!qs.isEmpty()) try {
				QFSqlQueryTable t;
				t.reload(qs);
				QFSValue tt = t.toTreeTable();
				//qfInfo() << doc.toString();
				QVariant v;
				v.setValue(tt);
				data_value.setValue(tt);
			}
			catch(QFException &e) {
				qfError() << "Report table data load error:" << e.msg();
			}
		}
	}
	else if(domain == S_DOMAIN_REPORT) {
		QString path = QFFileUtils::path(data_src);
		QString key = QFFileUtils::file(data_src);
		//qfDebug().noSpace() << "\t\tpath: '" << path << "'" << "\t\tname: '" << data << "'";
		QDomElement el = element.cd(path + "keyvals", !Qf::ThrowExc);
		if(!!el) {
			QFXmlKeyVals kv(el);
			data_value = kv.value(key, default_value);
		}
		else {
			qfWarning() << QString("Report path '%1' does not exist. Domain: %2 Element path: '%3'").arg(path + "keyvals").arg(domain).arg(element.path());
		}
	}
	else if(domain == S_DOMAIN_TABLE) {
		ReportItemBand *band = parentBand();
		if(!band) qfWarning().noSpace() << "'" << data_src << "' band is null";
		if(band) {
			if(data_src.startsWith("/")) {
				/// skoc na root band, coz je vlastne body
				while(true) {
					ReportItemBand *it = band->parentBand();
					if(!it) break;
					band = it;
				}
			}
			qfu::TreeTable t = band->dataTable();
			if(t.isNull()) {
				//qfInfo() << data_src << "table is NULL";
				if(info_if_not_found) data_value = '{' + data_src + '}';//qfWarning().noSpace() << "'" << data_src << "' table is null";
			}
			else {
				data_value = t.value(data_src, (info_if_not_found)? "$" + data_src: default_value, sql_match);
			}
		}
	}
	else if(domain == S_DOMAIN_ROW) {
		ReportItemDetail *det = currentDetail();
		if(det) {
			qfu::TreeTableRow r = det->dataRow();
			qfDebug() << "\t\tdata row is null:" << r.isNull();
			//if(data_src == "groupHeader") qfInfo() << data_src << "data_row:" << r.rowData().toString(2);
			if(r.isNull()) {
				if(info_if_not_found) data_value = '[' + data_src + ']';
			}
			else {
				if(data_src == S_TABLE_ROWNO) {
					data_value = det->currentRowNo() + 1;
				}
				else {
					data_value = r.value(data_src, (info_if_not_found)? "$" + data_src: default_value);
					//if(!data_value.isValid()) data_value = default_value;
					//qfInfo() << data_src << "=>" << data_value.toString() << "default value:" << ((info_if_not_found)? "$" + data_src: default_value).toString() << "info if not found:" << info_if_not_found;
				}
			}
		}
		else {
			if(info_if_not_found) data_value = "$" + data_src + " no detail";
		}
	}
	--*/
	//qfDebug() << "\treturn:" << data_value.toString() << QVariant::typeToName(data_value.type());
	//qfInfo() << "\treturn:" << data_value.toString() << QVariant::typeToName(data_value.type());
	return data_value;
}
#endif
/*
void ReportItem::setupMetaPaintItem(ReportItemMetaPaint *mpit)
{
	Q_UNUSED(mpit);
}
*/
void ReportItem::classBegin()
{
	qfLogFuncFrame();
}

void ReportItem::componentComplete()
{
	qfLogFuncFrame();
}

QString ReportItem::toString(int indent, int indent_offset)
{
	Q_UNUSED(indent);
	QString ret;
	QString indent_str;
	indent_str.fill(' ', indent_offset);
	ret += indent_str + metaObject()->className();
	return ret;
}

style::Text *ReportItem::effectiveTextStyle()
{
	style::Text *ret = nullptr;
	ReportItem *it = this;
	while(it) {
		ReportItemFrame *frit = qobject_cast<ReportItemFrame*>(it);
		if(frit) {
			ret = frit->textStyle();
			if(ret)
				break;
		}
		it = it->parent();
	}
	QF_ASSERT(ret != nullptr, "Cannot find TextStyle definition in parents", return ret);
	return ret;
}

//==========================================================
//                    ReportItemBreak
//==========================================================
ReportItemBreak::ReportItemBreak(ReportItem *parent)
	: Super(parent)
{
	/// attribut type (page | column) zatim nedela nic
	//QF_ASSERT_EX(proc, "processor is NULL", return);
	designedRect.verticalUnit = Rect::UnitInvalid;
	//qfInfo() << element.attribute("id");
	breaking = false;
}

ReportItem::PrintResult ReportItemBreak::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfLogFuncFrame();
	Q_UNUSED(bounding_rect);
	Q_UNUSED(out);
	PrintResult res = PrintOk;
	if(!isVisible()) { return res; }
	if(!breaking) {res = PrintNotFit; res.flags = FlagPrintBreak;}
	breaking = !breaking;
	return res;
}

//==========================================================
//                                    ReportItemBody
//==========================================================
/*
ReportItem::PrintResult ReportItemBody::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfDebug() << QF_FUNC_NAME;
	PrintResult res = ReportItemDetail::printMetaPaint(out, bounding_rect);
	/// body jediny ma tu vysadu, ze se muze vickrat za sebou nevytisknout a neznamena to print forever.
	if(res == PrintNeverFit) res = PrintNotFit;
	return res;
}
*/
#if 0
//==========================================================
//                                    ReportItemHeaderFrame
//==========================================================
ReportItem::PrintResult ReportItemHeaderFrame::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfDebug() << QF_FUNC_NAME;
	return ReportItemFrame::printMetaPaint(out, bounding_rect);
}

//==========================================================
//                                    ReportItemRow
//==========================================================
ReportItem::PrintResult ReportItemRow::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfDebug() << QF_FUNC_NAME;
	return ReportItemFrame::printMetaPaint(out, bounding_rect);
}

//==========================================================
//                                    ReportItemCell
//==========================================================
ReportItem::PrintResult ReportItemCell::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfDebug() << QF_FUNC_NAME;
	return ReportItemFrame::printMetaPaint(out, bounding_rect);
}
#endif
#ifdef REPORT_ITEM_TABLE
//==========================================================
//                                    ReportItemTable
//==========================================================
ReportItemTable::ReportItemTable(ReportItem *parent)
	: ReportItemBand(parent)
{
	//qfDebug() << QF_FUNC_NAME << "parent:" << parent();
	//QF_ASSERT(!!_el, "element is null.");
}

void ReportItemTable::syncChildren()
{
	qfLogFuncFrame() << this;
	deleteChildren();
	for(QDomElement el = fakeBand.firstChildElement(); !!el; el = el.nextSiblingElement()) {
		if(!ReportProcessor::isProcessible(el)) continue;
		/// vytvor chybejici item
		processor()->createProcessibleItem(el, this);
	}
	qfDebug() << QF_FUNC_NAME << "<<<<<<<<<<<<<<<< OUT";
}

void ReportItemTable::createFakeBand()
{
	//return;
	if(!!fakeBand) return;

	bool from_data = element.attribute("createfromdata").toBool();
	QString decoration = element.attribute("decoration");
	bool grid = (decoration == "grid");
	bool horizontal_lines = (decoration == "horizontallines");
	bool line = !decoration.isEmpty();
	QString line_pen = (grid)? "black1": "black1";
	bool shadow = element.attribute("shadow", "1").toBool();

	fakeBand = fakeBandDocument.createElement("band");
	fakeBandDocument.appendChild(fakeBand);
	fakeBand.setAttribute("__fake", 1); /// napomaha pri selekci v report editoru

	QDomElement el_header, el_detail, el_footer;
	{
		QDomElement el, el1;

		el1 = fakeBand.ownerDocument().createElement("row");
		el = element.firstChildElement("headerframe");
		el1.copyAttributesFrom(el);
		/// v tabulce ma smysl mit deti zarovnany
		if(grid && el1.attribute("expandChildrenFrames").isEmpty()) el1.setAttribute("expandChildrenFrames", "1");
		if(line && el1.attribute("bbrd").isEmpty()) el1.setAttribute("bbrd", line_pen);
		else if(horizontal_lines) el1.setAttribute("bbrd", line_pen);
		if(shadow && el1.attribute("fill").isEmpty()) el1.setAttribute("fill", "tblshadow");
		el_header = el1;
		el_header.setAttribute("__fakeBandHeaderRow", 1); /// napomaha exportu do HTML

		el1 = fakeBand.ownerDocument().createElement("detail");
		el = element.firstChildElement("detailframe");
		el1.copyAttributesFrom(el);
		if(grid && el1.attribute("expandChildrenFrames").isEmpty()) el1.setAttribute("expandChildrenFrames", "1");
		if(el1.attribute("keepall").isEmpty()) el1.setAttribute("keepall", "1");
		if(horizontal_lines) el1.setAttribute("bbrd", line_pen);
		el_detail = el1;
		el_detail.setAttribute("__fakeBandDetail", 1); /// napomaha exportu do HTML

		el1 = fakeBand.ownerDocument().createElement("row");
		el = element.firstChildElement("footerframe");
		el1.copyAttributesFrom(el);
		if(grid && el1.attribute("expandChildrenFrames").isEmpty()) el1.setAttribute("expandChildrenFrames", "1");
		if(line) el1.setAttribute("tbrd", line_pen);
		else if(horizontal_lines) el1.setAttribute("bbrd", line_pen);
		if(shadow && el1.attribute("fill").isEmpty()) el1.setAttribute("fill", "tblshadow");
		el_footer = el1;
		el_footer.setAttribute("__fakeBandFooterRow", 1); /// napomaha exportu do HTML
	}

	bool has_footer = false;
	if(from_data) {
		qfDebug() << "\tcreating from data";
		qfu::TreeTable t = dataTable();
		//foreach(const QFXmlTableColumnDef cd, t.columns()) if(!t.columnFooter(cd.name).isEmpty()) {has_footer = true; break;}
		qfu::TreeTableColumns cols = t.columns();
		for(int i=0; i<cols.count(); i++) {
			qfu::TreeTableColumn cd = cols.column(i);
			QString col_w = cd.width();
			if(col_w.isEmpty()) col_w = "%";
			{
				QDomElement el = fakeBand.ownerDocument().createElement("para");
				el.setAttribute("w", col_w);
				el.setAttribute("hinset", "1");
				el.setAttribute("style", "tblheading");
				el.setAttribute("halign", "center");
				if(grid) el.setAttribute("brd", "black1");
				//QDomText txt = fakeBand.ownerDocument().createTextNode(t.columnHeader(cd.name));
				//el.appendChild(txt);
				QDomElement el1 = fakeBand.ownerDocument().createElement("data");
				el1.setAttribute("src", "HEADER(" + cd.name() + ')');
				el1.setAttribute("domain", "table");
				el.appendChild(el1);
				el_header.appendChild(el);
			}
			{
				QDomElement el = fakeBand.ownerDocument().createElement("para");
				el.setAttribute("w", col_w);
				el.setAttribute("hinset", "1");
				el.setAttribute("style", "tbltext");
				el.setAttribute("halign", cd.hAlignment());
				if(grid) el.setAttribute("brd", "black1");
				QDomElement el1 = fakeBand.ownerDocument().createElement("data");
				el1.setAttribute("src", cd.name());
				el.appendChild(el1);
				el_detail.appendChild(el);
			}
			{
				QString footer = t.columnFooter(cd.name());
				if(!footer.isEmpty()) has_footer = true;
				QDomElement el = fakeBand.ownerDocument().createElement("para");
				el.setAttribute("w", col_w);
				el.setAttribute("hinset", "1");
				el.setAttribute("style", "tbltextB");
				el.setAttribute("halign", cd.hAlignment());
				if(grid) el.setAttribute("brd", "black1");
				//QDomText txt = fakeBand.ownerDocument().createTextNode(footer);
				//el.appendChild(txt);
				if(has_footer) {
					QDomElement el1 = fakeBand.ownerDocument().createElement("data");
					el1.setAttribute("src", "FOOTER(" + cd.name() + ')');
					el1.setAttribute("domain", "table");
					el.appendChild(el1);
				}
				el_footer.appendChild(el);
			}
		}
	}
	else {
		QDomElement el = element.firstChildElement("cols");
		int col_no = 0;
		for(el=el.firstChildElement("col"); !!el; el=el.nextSiblingElement("col")) {
			QString fake_path = QString("cols/col[%1]").arg(col_no);
			QDomElement el1;
			el1 = el.firstChildElement("colheader");
			if(!!el1) {
				el1 = el1.cloneNode().toElement();
				el1.setTagName("cell");
				el1.setAttribute("__fakePath", fake_path + "/colheader");
			}
			else {
				el1 = fakeBand.ownerDocument().createElement("cell");
				el1.setAttribute("__fakePath", fake_path);
			}
			el1.copyAttributesFrom(el);
			if(grid) el1.setAttribute("brd", "black1");
			el_header.appendChild(el1);

			el1 = el.firstChildElement("coldetail");
			if(!!el1) {
				el1 = el1.cloneNode().toElement();
				el1.setTagName("cell");
				if(grid) el1.setAttribute("brd", "black1");
				el1.setAttribute("__fakePath", fake_path + "/coldetail");
			}
			else {
				el1 = fakeBand.ownerDocument().createElement("cell");
				el1.setAttribute("__fakePath", fake_path);
			}
			el1.copyAttributesFrom(el);
			el_detail.appendChild(el1);

			el1 = el.firstChildElement("colfooter");
			if(!!el1) {
				has_footer = true;
				el1 = el1.cloneNode().toElement();
				el1.setTagName("cell");
				if(grid) el1.setAttribute("brd", "black1");
				el1.setAttribute("__fakePath", fake_path + "/colfooter");
			}
			else {
				el1 = fakeBand.ownerDocument().createElement("cell");
				el1.setAttribute("__fakePath", fake_path);
			}
			el1.copyAttributesFrom(el);
			el_footer.appendChild(el1);
			col_no++;
		}
	}

	QDomElement el_columns;
	for(QDomElement el=element.firstChildElement(); !!el; el=el.nextSiblingElement()) {
		if(el.tagName() == "cols") {
			el_columns = el;
			break;
		}
		QDomElement el1 = el.cloneNode().toElement();
		fakeBand.appendChild(el1);
	}
	fakeBand.appendChild(el_header);
	fakeBand.appendChild(el_detail);
	if(has_footer) fakeBand.appendChild(el_footer);
	for(QDomElement el=el_columns.nextSiblingElement(); !!el; el=el.nextSiblingElement()) {
		QDomElement el1 = el.cloneNode().toElement();
		fakeBand.appendChild(el1);
	}
	//qfInfo() << fakeBand.toString();
}

ReportItem::PrintResult ReportItemTable::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	createFakeBand();
	return ReportItemBand::printMetaPaint(out, bounding_rect);
}
#endif

//===============================================================
//                ReportItemGraph
//===============================================================
#if 0
void ReportItemGraph::syncChildren()
{
	qfDebug() << QF_FUNC_NAME << element.tagName() << "id:" << element.attribute("id");
	static int graph_no = 0;
	src = QString("key:/graph-%1").arg(++graph_no);

	childrenSyncedFlag = true;
	ReportItem::syncChildren();
}
#endif

#ifdef REPORT_ITEM_GRAPH
ReportItemImage::PrintResult ReportItemGraph::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	PrintResult res = PrintOk;
	Rect br = bounding_rect;

	/// vykresli graf a pridej ho do processor()->images()
	QPicture pict;
	QVariantMap definition;
	qfError() << "Graphs are not fully implemented yet.";
	graphics::Graph *graph = graphics::Graph::createGraph(definition, findDataTable(dataSource()));
	if(graph) {
		graph->setStyleCache(processor()->context().styleCache());
		QPainter painter;
		painter.begin(&pict);
		graph->draw(&painter, br.size());
		painter.end();

		/// nastav velikost grafu v pixelech na bounding rect, bez ohledu na to, co vymysli QPainter
		/// nevim proc, ale u grafu si mysli, ze pokreslil jenom gridrect
		/// vypada to, jako ze tisk textu se do boundingrect nepocita

		/// jako zaklad vem rect, ktery je predepsan v reportu, do nej by se mel graf vejit
		QRect r = qmlwidgets::graphics::mm2device(br, &pict).toRect();
		/// bounding rect obrazku grafu je vzdy od [0,0]
		r.moveTo(0, 0);
		//qfInfo() << "graph bounding_rect:" << Rect(r).toString();
		//qfInfo() << "pict bounding_rect:" << Rect(pict.boundingRect()).toString();
		/// muze se stat, ze treba data jsou tak rozsahla, ze presahnou ramecek grafu
		/// v takovem pripade zvets popsany ctverec o tyto pripadne presahy
		/// vysledkem teto akce je, ze graf se vzdy vejde do predepsaqneho ramecku, bez ohledu na to, jestli ho pri vykreslovani nekde nepretah
		/// pokud ho pretahne, skutecne popsana plocha se o toto pretazeni zvetsi
		/// pokud bych zakomentoval nasledujici radek, sloupce caroveho grafu by pro urcite kombinace vykreslovanych dat mohly vylezt nad region urceny pro graf
		/// (napr. v pripade, ze je vice serii a osy se nastavi podle serie s mensi max. hodnotou nez maji ostatni serie (plan obchodu - sestava plneni planu))
		r = r.united(pict.boundingRect());
		pict.setBoundingRect(r);
		//qfInfo() << "pict united rect:" << Rect(pict.boundingRect()).toString();
		processor()->addImage(m_resolvedDataSource, ReportItem::Image(pict));
		delete graph;
	}
	//qfInfo() << "physicalDpiX:" << pict.physicalDpiX();
	//qfInfo() << "logicalDpiX:" << pict.logicalDpiX();

	res = ReportItemImage::printMetaPaintChildren(out, br);
	return res;
}
#endif
