
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#include "reportpainter.h"
#include "reportprocessor.h"
#include "reportitem.h"
#include "style/compiledtextstyle.h"
//#include "../../graphics/graph/graph.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>
#include <qf/core/utils/fileutils.h>

#include <QDate>
#include <QCryptographicHash>
#include <QSvgRenderer>
#include <QBuffer>
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

ReportItemBand* ReportItem::parentBand()
{
	ReportItem *it = this->parent();
	while(it) {
		if(it->toBand()) return it->toBand();
		it = it->parent();
	}
	return NULL;
}

ReportItemDetail* ReportItem::currentDetail()
{
	ReportItem *it = const_cast<ReportItem *>(this);
	while(it) {
		if(it->toDetail()) return it->toDetail();
		it = it->parent();
	}
	return NULL;
}

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

void ReportItem::setupMetaPaintItem(ReportItemMetaPaint *mpit)
{
	Q_UNUSED(mpit);
}

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
//                                    ReportItemFrame
//==========================================================
ReportItemFrame::ReportItemFrame(ReportItem *parent)
	: Super(parent)
{
	qfLogFuncFrame();
	//m_x1 = -1;
	//m_y1 = -1;
	//m_x2 = -1;
	//m_y2 = -1;
	m_hinset = m_vinset = 0;
	m_layout = LayoutVertical;
	m_expandChildrenFrames = false;
	m_horizontalAlignment = AlignLeft;
	m_verticalAlignment = AlignTop;

	indexToPrint = 0;
	//qfDebug() << QF_FUNC_NAME << "*******************" << el.tagName() << el.attribute("id");
}

ReportItemFrame::~ReportItemFrame()
{
	qfLogFuncFrame();
}

void ReportItemFrame::componentComplete()
{
	// update designed rect
	/*--
	Point p;
	if(x1() >= 0) {
		designedRect.flags |= Rect::LeftFixed;
		p.rx() = x1();
	}
	if(y1() >= 0) {
		designedRect.flags |= Rect::TopFixed;
		p.ry() = y1();
	}
	designedRect.setTopLeft(p);
	if(x2() >= 0) {
		designedRect.flags |= Rect::RightFixed;
		p.rx() = x2();
	}
	if(y2() >= 0) {
		designedRect.flags |= Rect::BottomFixed;
		p.ry() = y2();
	}
	designedRect.setBottomRight(p);
	--*/
	//qfDebug() << "\t" << __LINE__ << "designedRect:" << designedRect.toString();
	//static const QString S_PERCENT = "%";
	{
		QVariant v = width();
		qreal d = 0;
		bool ok;
		if(v.type() == QVariant::String) {
			qfc::String s;
			s = v.toString().trimmed();
			{
				if(s.value(-1) == '%') {
					s = s.slice(0, -1).trimmed();
					designedRect.horizontalUnit = Rect::UnitPercent;
				}
				if(!s.isEmpty()) {
					d = s.toDouble(&ok);
					if(!ok)
						qfWarning() << "Cannot convert" << s << "to real number." << this;
				}
				/*--
				if(d > 0) {
					if(designedRect.flags & Rect::RightFixed) {
						qreal r = designedRect.right();
						designedRect.setWidth(d);
						designedRect.moveRight(r);
					}
					else
						designedRect.setWidth(d);
				}
				--*/
			}
		}
		else if(v.isValid()) {
			d = v.toReal(&ok);
			if(!ok)
				qfWarning() << "Cannot convert" << v.toString() << "to real number." << this;
		}
		designedRect.setWidth(d);
	}
	{
		QVariant v = height();
		qreal d = 0;
		bool ok;
		if(v.type() == QVariant::String) {
			qfc::String s;
			s = v.toString().trimmed();
			{
				if(s.value(-1) == '%') {
					s = s.slice(0, -1).trimmed();
					designedRect.horizontalUnit = Rect::UnitPercent;
				}
				if(!s.isEmpty()) {
					d = s.toDouble(&ok);
					if(!ok)
						qfWarning() << "Cannot convert" << s << "to real number." << this;
				}
				/*--
				if(d > 0) {
					if(designedRect.flags & Rect::BottomFixed) {
						qreal b = designedRect.bottom();
						designedRect.setWidth(d);
						designedRect.moveBottom(b);
					}
					else
						designedRect.setHeight(d);
				}
				--*/
			}
		}
		else if(v.isValid()) {
			d = v.toReal(&ok);
			if(!ok)
				qfWarning() << "Cannot convert" << v.toString() << "to real number." << this;
		}
		designedRect.setHeight(d);
	}

	if(isExpandChildrenFrames()) {
		designedRect.flags |= Rect::ExpandChildrenFrames;
	}

	if(layout() == LayoutHorizontal)
		designedRect.flags |= Rect::LayoutHorizontalFlag;
	else
		designedRect.flags |= Rect::LayoutVerticalFlag;

	qfDebug() << "\tdesignedRect:" << designedRect.toString();
}

QString ReportItemFrame::toString(int indent, int indent_offset)
{
	QString ret = Super::toString(indent, indent_offset);
	for(int i=0; i<itemCount(); i++) {
		ret += '\n';
		ReportItem *it = itemAt(i);
		ret += it->toString(indent, indent_offset += indent);
	}
	return ret;
}

QQmlListProperty<ReportItem> ReportItemFrame::items()
{
	return QQmlListProperty<ReportItem>(this, 0,
                                    ReportItemFrame::addItemFunction,
                                    ReportItemFrame::countItemsFunction,
                                    ReportItemFrame::itemAtFunction,
                                    ReportItemFrame::removeAllItemsFunction
										);
}

void ReportItemFrame::addItemFunction(QQmlListProperty<ReportItem> *list_property, ReportItem *item)
{
	if (item) {
		ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
		item->setParent(that);
		that->m_items << item;
    }
}

ReportItem *ReportItemFrame::itemAtFunction(QQmlListProperty<ReportItem> *list_property, int index)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	return that->m_items.value(index);
}

void ReportItemFrame::removeAllItemsFunction(QQmlListProperty<ReportItem> *list_property)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	qDeleteAll(that->m_items);
	that->m_items.clear();
}

int ReportItemFrame::countItemsFunction(QQmlListProperty<ReportItem> *list_property)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	return that->itemCount();
}

int ReportItemFrame::itemCount() const
{
	return m_items.count();
}

ReportItem *ReportItemFrame::itemAt(int index)
{
	return m_items[index];
}

void ReportItemFrame::setupMetaPaintItem(ReportItemMetaPaint *mpit)
{
	Super::setupMetaPaintItem(mpit);
	style::Text *pts = effectiveTextStyle();
	if(pts) {
		style::CompiledTextStyle ts = pts->textStyle();
		mpit->setTextStyle(ts);
	}
}

ReportItem::ChildSize ReportItemFrame::childSize(Layout parent_layout)
{
	if(parent_layout == LayoutHorizontal)
		return ChildSize(designedRect.width(), designedRect.horizontalUnit);
	return ChildSize(designedRect.height(), designedRect.verticalUnit);
}

ReportItem::PrintResult ReportItemFrame::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame();// << element.tagName() << "id:" << element.attribute("id") << "itemCount:" << itemCount() << "indexToPrint:" << indexToPrint;
	qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	PrintResult res = PrintOk;
	//dirtySize = Size();
	Rect bbr = bounding_rect;
	//Size children_dirty_size;
	//Size children_bounding_size = bbr.size();

	if(layout() == LayoutHorizontal) {
		/// horizontalni layout musi mit procenta rozpocitany dopredu, protoze jinak by se mi nezalamovaly texty v tabulkach
		/// v horizontalnim layoutu break ignoruj
		QList<ChildSize> sizes;
		/// pri tisku horizontalniho layoutu se tiskne vzdy od zacatku
		indexToPrint = 0;
		/*--
		ReportItemFrame *parent_grid = NULL;
		if(isParentGridLayout()) {
			ReportItem *it = this->parent();
			while(it) {
				ReportItemFrame *frm_it = qobject_cast<ReportItemFrame*>(it);
				if(frm_it && frm_it->isParentGrid()) {
					parent_grid = frm_it;
					break;
				}
				it = it->parent();
			}
		}
		bool is_first_grid_child = (parent_grid && parent_grid->gridLayoutSizes().isEmpty());
		bool is_next_grid_child = (parent_grid && !is_first_grid_child);

		if(is_next_grid_child) {
			/// dalsi deti gridu si zkopiruji rozmery z prvniho ditete, ktere je jiz vytisknute
			QList<double> szs = parent_grid->gridLayoutSizes();
			for(int i=indexToPrint; i<childrenCount(); i++) {
				ReportItem *it = itemAt(i);
				ChildSize sz;
				if(i < szs.count()) {
					it->designedRect.horizontalUnit = Rect::UnitMM;
					it->designedRect.setWidth(szs[i]); /// nastav mu velikost z prvniho tisku
				}
				else {
					qfWarning() << "this should never happen with grid layout, grid layout rows have different cell numbers, falling to original size values.";
				}
			}
		}
		--*/
		{
			/// prvni dite gridu se tiskne jako vse ostatni
			/// nastav detem mm rozmer ve smeru layoutu
			for(int i=indexToPrint; i<itemCount(); i++) {
				ReportItem *it = itemAt(i);
				sizes << it->childSize(layout());
			}
		}

		/// zbyva vypocitat jeste ortogonalni rozmer
		/// je to bud absolutni hodnota nebo % z bbr
		QList<ChildSize> orthogonal_sizes;
		for(int i=indexToPrint; i<itemCount(); i++) {
			ReportItem *it = itemAt(i);
			Layout ol = orthogonalLayout();
			ChildSize sz = it->childSize(ol);
			if(sz.unit == Rect::UnitPercent) {
				sz.size = bbr.sizeInLayout(ol); /// udelej z nej rubber, roztahne se dodatecne
				//if(sz.size == 0) sz.size = bbr.sizeInLayout(ol);
				//else sz.size = sz.size / 100 * bbr.sizeInLayout(ol);
			}
			orthogonal_sizes << sz;
			//if(it->isBreak() && i > indexToPrint && layout() == LayoutVertical) break; /// v horizontalnim layoutu break ignoruj
		}

		// can_print_children_in_natural_order je true v pripade, ze procenta nejsou nebo 1. procenta jsou za poslednim rubber frame.
		//bool can_print_children_in_natural_order = first_percent_ix < 0 || last_rubber_ix < 0 || last_rubber_ix < first_percent_ix;

		{
			/// v horizontalnim layoutu vytiskni nejdriv fixed itemy, pak rubber, ze zbytku rozpocitej % a vytiskni je taky
			/// vytiskly itemy pak rozsoupej do spravnyho poradi

			QMap<int, int> poradi_tisku; /// layout_ix->print_ix
			qreal sum_mm = 0;
			bool has_percent = false;
			/// vytiskni rubber a fixed
			//if(parent_grid) qfWarning() << (is_first_grid_child? "first child": is_next_grid_child? "next child": "nevim");
			for(int i=0; i<itemCount(); i++) {
				ReportItem *it = itemAt(i);
				ChildSize sz = sizes.value(i);
				//qfInfo() << "child:" << i << "size:" << sz.size << "unit:" << Rect::unitToString(sz.unit);
				if(sz.unit == Rect::UnitMM) {
					Rect ch_bbr = bbr;
					ch_bbr.setLeft(bbr.left() + sum_mm);
					if(sz.size > 0) ch_bbr.setWidth(sz.size);
					else ch_bbr.setWidth(bbr.width() - sum_mm);
					if(orthogonal_sizes[i].size > 0) {
						ch_bbr.setSizeInLayout(orthogonal_sizes[i].size, orthogonalLayout());
					}
					//qfInfo() << "\t tisknu fixed:" << it->designedRect.toString();
					int prev_children_cnt = out->childrenCount();
					PrintResult ch_res = it->printMetaPaint(out, ch_bbr);
					if(out->children().count() > prev_children_cnt) {
						//qfInfo() << "rubber fixed:" << i << "->" << prev_children_cnt;
						poradi_tisku[i] = prev_children_cnt;
						double width = out->lastChild()->renderedRect.width();
						sum_mm += width;
						//if(parent_grid) qfInfo() << "\t renderedRect:" << out->lastChild()->renderedRect.toString();
						//}
						//qfInfo() << "\t sum_mm:" << sum_mm;
						if(ch_res.value != PrintOk) {
							/// para se muze vytisknout a pritom bejt not fit, pokud pretece
							res = ch_res;
						}
					}
					else {
						if(ch_res.value == PrintOk) {
							/// jediny, kdo se nemusi vytisknout je band
							if(!qobject_cast<ReportItemBand*>(it)) {
								qfWarning() << "jak to, ze se dite nevytisklo v horizontalnim layoutu?" << it;
							}
						}
						else {
							//qfInfo() << "\t NOT OK";
							res = ch_res;
							break;
						}
					}
				}
				else {
					has_percent = true;
				}
			}
			qreal rest_mm = bounding_rect.width() - sum_mm;

			if(res.value == PrintOk) {
				if(has_percent) {
					/// rozpocitej procenta
					qreal sum_percent = 0;
					int cnt_0_percent = 0;
					for(int i=0; i<itemCount(); i++) {
						ReportItem *it = itemAt(i);
						ChildSize sz = it->childSize(layout());
						if(sz.unit == Rect::UnitPercent) {
							if(sz.size == 0) cnt_0_percent++;
							else sum_percent += sz.size;
						}
					}
					if(rest_mm <= 0) {
						qfWarning() << "Percent exist but rest_mm is" << rest_mm << ". Ignoring rest of frames";
					}
					else {
						/// vytiskni procenta
						qreal percent_0 = 0;
						if(cnt_0_percent > 0) percent_0 = (100 - sum_percent) / cnt_0_percent;
						for(int i=0; i<itemCount(); i++) {
							ReportItem *it = itemAt(i);
							ChildSize sz = it->childSize(layout());
							if(sz.unit == Rect::UnitPercent) {
								qreal d;
								if(sz.size == 0) d = rest_mm * percent_0 / 100;
								else d = rest_mm * sz.size / 100;
								//qfInfo() << d;
								Rect ch_bbr = bbr;
								ch_bbr.setWidth(d);
								if(orthogonal_sizes[i].size > 0) {
									ch_bbr.setSizeInLayout(orthogonal_sizes[i].size, orthogonalLayout());
								}
								//qfInfo() << it << "tisknu percent" << it->designedRect.toString();
								//qfInfo() << "chbr" << ch_bbr.toString();
								int prev_children_cnt = out->childrenCount();
								PrintResult ch_res = it->printMetaPaint(out, ch_bbr);
								if(out->children().count() > prev_children_cnt) {
									//qfInfo() << "percent:" << i << "->" << prev_children_cnt;
									poradi_tisku[i] = prev_children_cnt;
									if(ch_res.value != PrintOk) {
										/// para se muze vytisknout a pritom bejt not fit, pokud pretece
										res = ch_res;
									}
								}
								else {
									if(ch_res.value == PrintOk) {
										/// jediny, kdo se nemusi vytisknout je band
										if(!qobject_cast<ReportItemBand*>(it)) {
											qfWarning() << "jak to, ze se dite nevytisklo v horizontalnim layoutu?" << it;
										}
									}
									else { res = ch_res; break; }
								}
							}
						}
					}
					/// posprehazej vytisknuty deti
					//qfInfo() << "\t poradi tisku cnt:<<" << poradi_tisku.count() << out->childrenCount();
					if(poradi_tisku.count() == out->children().count()) {
						int children_count = out->children().count();
						//qfInfo() << "children cnt:" << children_count;
						//QF_ASSERT(poradi_tisku.count() == out->children().count(), "nevytiskly se vsechny deti v horizontalnim layoutu");
						QVector<qf::core::utils::TreeItemBase*> old_children(poradi_tisku.count());
						/// zkopiruj ukazatele na deti
						for(int i=0; i<children_count; i++)
							old_children[i] = out->children()[i];
						/// dej je do spravnyho poradi
						/// v mape nemusi byt rada klicu souvisla (kdyz se nejake dite nevytiskne)
						QMapIterator<int, int> iter(poradi_tisku);
						int new_print_ix = 0;
						while(iter.hasNext()) {
							iter.next();
							int old_print_ix = iter.value();
							if(0 <= new_print_ix && new_print_ix < children_count) {
								//qfInfo() << old_print_ix << "->" << new_print_ix;
								if(new_print_ix != old_print_ix) out->childrenRef()[new_print_ix] = old_children[old_print_ix];
								new_print_ix++;
							}
							else qfWarning() << QF_FUNC_NAME << "poradi:" << old_print_ix << "new_ix:" << new_print_ix << "out of range:" << children_count;
						}

						/// nastav detem spravne offsety
						qreal offset_x = 0;
						for(int i=0; i<poradi_tisku.count(); i++) {
							//qfInfo() << "\t poradi tisku <<" << i << "offset:" << offset_x;
							ReportItemMetaPaint *it = out->child(i);
							/// tady je to potreba posunout vcetne deti :(
							double shift_x = bbr.left() + offset_x - it->renderedRect.left();
							//if(parent_grid) qfInfo() << i << "offset_x:" << offset_x << "bbr left:" << bbr.left() << "chbbr left:" << ch_bbr.left();
							if(qFloatDistance(shift_x, 0) > 200)
								it->shift(Point(shift_x, 0));
							offset_x += it->renderedRect.width();
						}
					}
					else {
						qfWarning() << this << "nesedi poradi pocty tisku" << poradi_tisku.count() << out->children().count();
					}
				}
				/*--
				if(is_first_grid_child) {
					QList<double> szs;
					for(int i=0; i<out->children().count(); i++) {
						//qfInfo() << "\t poradi tisku <<" << i << "offset:" << offset_x;
						ReportItemMetaPaint *it = out->child(i);
						szs << it->renderedRect.width();
					}
					parent_grid->setGridLayoutSizes(szs);
				}
				--*/
			}
			if(res.value != PrintOk) {
				/// detail by mel mit, pokud se ma zalamovat, vzdy vertikalni layout, jinak tato funkce zpusobi, ze se po zalomeni vsechny dcerine bandy budou tisknout cele znova
				/// zakomentoval jsem to a zda se, ze to zatim nicemu nevadi
				//resetIndexToPrintRecursively(!ReportItem::IncludingParaTexts);
			}
		}
	}
	else {
		/// vertikalni layout
		/// tiskni
		/// procenta tiskni jako rubber a v pripade print OK je roztahni v metapaintu
		//qreal length_mm = bbr.sizeInLayout(layout);
		/// break funguje tak, ze pri 1., 3., 5. atd. tisku vraci PrintNotFit a pri sudych PrintOk
		/// prvni break na strance znamena, ze jsem tu uz po zalomeni, takze se tiskne to za break.
		//if(it->isBreak() && i > indexToPrint && layout == LayoutVertical) break;
		int index_to_print_0 = indexToPrint;
		for(; indexToPrint<itemCount(); indexToPrint++) {
			ReportItem *it = itemAt(indexToPrint);
			Rect ch_bbr = bbr;
			//bool item_is_rubber_in_layout = false;
			qfDebug() << "\tch_bbr v1:" << ch_bbr.toString();

			{
				/// vymysli rozmer ve smeru layoutu
				qreal d = ch_bbr.sizeInLayout(layout());
				//qfInfo() << "indexToPrint:" << indexToPrint << "index_to_print_0:" << index_to_print_0 << "sizes.count():" << sizes.count();
				ChildSize sz = it->childSize(layout());
				if(sz.fillLayoutRatio() >= 0) {
					//item_is_rubber_in_layout = true;
					//fill_vertical_layout_ratio = sz.size / 100.;
					//sz.size = 0;
				}
				else if(sz.unit == Rect::UnitMM) {
					if(sz.size > 0) d = sz.size;
					//else item_is_rubber_in_layout = true;
				}
				else {
					ReportItemFrame *frit = qobject_cast<ReportItemFrame*>(it);
					if(frit)
						qfWarning() << "tohle by se asi nemelo stat" << it;
				}
				d = qMin(ch_bbr.sizeInLayout(layout()), d);
				ch_bbr.setSizeInLayout(d, layout());
			}
			{
				/// orthogonal size
				Layout ol = orthogonalLayout();
				ChildSize o_sz = it->childSize(ol);
				if(o_sz.unit == Rect::UnitPercent) {
					if(o_sz.size == 0) o_sz.size = bbr.sizeInLayout(ol);
					else o_sz.size = o_sz.size / 100 * bbr.sizeInLayout(ol);
				}
				//it->metaPaintOrthogonalLayoutLength = sz.size;
				qfDebug() << "\tsetting orthogonal length:" << o_sz.size;
				//if(it->isBreak() && i > indexToPrint && layout == LayoutVertical) break; /// v horizontalnim layoutu break ignoruj
				if(o_sz.size > 0) {
					ch_bbr.setSizeInLayout(o_sz.size, orthogonalLayout());
				}
			}
			qfDebug() << "\tch_bbr v2:" << ch_bbr.toString();
			int prev_children_cnt = out->childrenCount();
			PrintResult ch_res = it->printMetaPaint(out, ch_bbr);
			if(ch_res.value == PrintOk) {
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
				//dirtyRect = dirtyRect.unite(it->dirtyRect);
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
				/// muze se stat, ze se dite nevytiskne, napriklad band nema zadna data
				//QF_ASSERT(out->children().count() > 0, "jak to, ze se dite nevytisklo?");
				if(out->children().count() > prev_children_cnt) {
					ReportItemMetaPaint *mpi = out->lastChild();
					//if(fill_vertical_layout_ratio >= 0) mpi->setFillVLayoutRatio(fill_vertical_layout_ratio);
					if(mpi) {
						const Rect &r = mpi->renderedRect;
						//qfInfo() << mpi << mpi->reportItem()->element.tagName() << (r.flags & Rect::BackgroundItem) << "\tr:" << r.toString() << "ch_res:" << ch_res.toString();
						//if((r.flags & Rect::BackgroundItem)) qfWarning() << "BackgroundItem";
						//--if(!(r.flags & Rect::BackgroundItem))
						bbr.cutSizeInLayout(r, layout());
						if(ch_res.flags & FlagPrintAgain) {
							indexToPrint--; /// vytiskni ho znovu
						}
					}
				}
				//bbr.cutSizeInLayout(it->dirtyRect, layout);
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
			}
			else {
				/// pokud je vertikalni layout, a dite se nevejde vrat PrintNotFit
				if(layout() == LayoutHorizontal) {
					/// v horizontalnim, zadne pretikani neni
					/// vytiskni to znovu s doteklymi texty
					qfError() << "AHA??? this should never happen, I'm already in the vertical layout'";
					resetIndexToPrintRecursively(!ReportItem::IncludingParaTexts);
				}
				res = ch_res;
				break;
			}
			if(it->isBreak() && indexToPrint > index_to_print_0 && layout() == LayoutVertical) break;
		}
	}
	//res = checkPrintResult(res);
	qfDebug() << "\t<<< CHILDREN return:" << res.toString();
	return res;
}

ReportItem::PrintResult ReportItemFrame::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << element.tagName() << "id:" << element.attribute("id") << "designedRect:" << designedRect.toString();
	qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	qfDebug() << "\tdesignedRect:" << designedRect.toString();// << "isLeftTopFloating:" << isLeftTopFloating() << "isRightBottomFloating:" << isRightBottomFloating();
	qfDebug() << "\tlayout:" << ((layout() == LayoutHorizontal)? "horizontal": "vertical") << ", is rubber:" << isRubber(layout());
	PrintResult res = PrintOk;
	//--updateChildren();
	if(!isVisible())
		return res;
	Rect frame_content_br = bounding_rect;
	qfDebug() << "\tbbr 0:" << frame_content_br.toString();
	/*--
	if(designedRect.isAnchored()) {
		/// pokud je designedRect anchored neni treba ho nekam cpat
		/// pokud frame neni floating, vyprdni se na bounding_rect
		frame_content_br = designedRect;
		if(frame_content_br.isRubber(LayoutHorizontal)) frame_content_br.setRight(bounding_rect.right());
		if(frame_content_br.isRubber(LayoutVertical)) frame_content_br.setBottom(bounding_rect.bottom());
	}
	else
	--*/
	{
		if(designedRect.horizontalUnit == Rect::UnitMM && designedRect.width() - Epsilon > bounding_rect.width()) {
			qfDebug() << "\t<<<< FRAME NOT FIT WIDTH";
			return checkPrintResult(PrintNotFit);
		}
		if(designedRect.verticalUnit == Rect::UnitMM && designedRect.height() - Epsilon > bounding_rect.height()) {
			qfDebug() << "\t<<<< FRAME NOT FIT HEIGHT";
			//qfInfo() << "\tbounding_rect:" << bounding_rect.toString() << "height:" << bounding_rect.height();
			//qfInfo() << "\tdesignedRect:" << designedRect.toString() << "height:" << designedRect.height();
			//qfInfo() << "\tbounding_rect.height() < designedRect.height() (" << bounding_rect.height() << "<" << designedRect.height() << "):" << (bounding_rect.height() > designedRect.height());
			return checkPrintResult(PrintNotFit);
		}
	}
	frame_content_br.adjust(hinset(), vinset(), -hinset(), -vinset());

	QList<double> column_sizes;
	double columns_gap = 0;
	//int current_column_index;
	if(column_sizes.isEmpty()) {
		qf::core::String s = columns();
		QStringList sl = s.splitAndTrim(',');
		columns_gap = columnsGap();
		double ly_size = frame_content_br.width() - (columns_gap * (sl.count() - 1));
		column_sizes = qf::qmlwidgets::graphics::makeLayoutSizes(sl, ly_size);
		//current_column_index = 0;
	}
	ReportItemMetaPaintFrame *mp = new ReportItemMetaPaintFrame(out, this);
	QF_ASSERT_EX(mp != nullptr, "Meta paint item for item " + QString(this->metaObject()->className()) + " not created.");
	mp->setInset(hinset(), vinset());
	mp->setLayout((qf::qmlwidgets::graphics::Layout)layout());
	mp->setAlignment(horizontalAlignment(), verticalAlignment());
	Rect column_br_helper = frame_content_br;
	for(int current_column_index=0; current_column_index<column_sizes.count(); current_column_index++) {
		Rect column_br = column_br_helper;
		column_br.setWidth(column_sizes.value(current_column_index));
		qfDebug() << "\tcolumn bounding rect:" << column_br.toString();

		res = printMetaPaintChildren(mp, column_br);
		//qfDebug() << "\tbbr_init:" << bbr_init.toString();

		if(res.value == PrintNotFit) {
			//qfInfo().color(QFLog::Yellow) << element.tagName() << "keep all:" << keepAll << "column" << current_column_index << "of" << column_sizes.count();
			//qfInfo().color(QFLog::Yellow) << "column_br:" << column_br.toString() << "frame_content_br:" << frame_content_br.toString();
			/// pokud je result neverfit, nech ho tam, at aspon vidime, co se nikdy nevejde
			if(isKeepAll() && !(res.flags & FlagPrintNeverFit)) {
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				//qfInfo() << "keepAll && !(res.flags & FlagPrintNeverFit)";
				QF_SAFE_DELETE(mp);
				//qfInfo().color(QFLog::Green) << "return" << current_column_index << "of" << column_sizes.count();
				return checkPrintResult(res);
			}
		}
#if 0
		/*if(!isRubber(layout))*/ {
			/// pokud ma nektere dite flag filllayout, roztahni ho tak aby s ostatnimi detmi vyplnili layout
			/// zatim nevim proc, ale funguje to dobre jedine kdyz maji vsechny deti % nebo absolutni rozmer, kdyz je nejaky dite ve smeru layoutu rubber, tak to pocita spatne
			int filllayout_child_ix = -1;
			qreal sum_mm = 0;
			for(int i=0; i<mp->childrenCount(); i++) {
				ReportItemMetaPaint *it = mp->child(i);
				if(it->renderedRect.flags & Rect::FillLayout) filllayout_child_ix = i;
				qreal ly_sz = it->renderedRect.sizeInLayout(layout);
				sum_mm += ly_sz;
				//qfInfo() << i << ly_sz;
			}
			if(filllayout_child_ix >= 0) {
				//qfWarning() << "sum_mm:" << sum_mm;
				qreal offset = column_br.sizeInLayout(layout) - sum_mm;
				if(offset > 0) {
					ReportItemMetaPaint *it = mp->child(filllayout_child_ix);
					it->renderedRect.setSizeInLayout(it->renderedRect.sizeInLayout(layout) + offset, layout);
					it->alignChildren();
					Point p;
					if(layout == LayoutHorizontal) p.setX(offset);
					else if(layout == LayoutVertical) p.setY(offset);
					for(int i=filllayout_child_ix + 1; i<mp->childrenCount(); i++) {
						it = mp->child(i);
						it->shift(p);
					}
				}
			}
		}
#endif
		column_br_helper = column_br;
		column_br_helper.moveLeft(column_br.right() + columns_gap);
	}

	/// tak kolik jsem toho pokreslil?
	Rect dirty_rect;//, rendered_rect = designedRect;
	dirty_rect.flags = designedRect.flags;
	{
		/// musim to proste secist
		for(int i=0; i<mp->childrenCount(); i++) {
			ReportItemMetaPaint *it = mp->child(i);
			//qfInfo() << "child" << i << "rendered rect:" << it->renderedRect.toString() << "is null:" << it->renderedRect.isNull();
			//qfInfo() << "\t 1 rubber dirty_rect:" << dirty_rect.toString();
			if(dirty_rect.isNull()) dirty_rect = it->renderedRect;
			else dirty_rect = dirty_rect.united(it->renderedRect);
			//dirty_rect.flags |= it->renderedRect.flags;
			//qfInfo() << "\t 2 rubber dirty_rect:" << dirty_rect.toString();
		}
		qfDebug() << "\trubber dirty_rect:" << dirty_rect.toString();
	}
	//qfWarning() << this << "\tdirty_rect 1:" << dirty_rect.toString();
	/// pokud je v nekterem smeru definovany, je jedno, kolik se toho potisklo a nastav ten rozmer
	if(designedRect.horizontalUnit == Rect::UnitPercent)
		dirty_rect.setWidth(frame_content_br.width()); /// horizontalni rozmer musi ctit procenta
	else if(designedRect.horizontalUnit == Rect::UnitMM && designedRect.width() > 0)
		dirty_rect.setWidth(designedRect.width() - 2*hinset());
	if(designedRect.verticalUnit == Rect::UnitMM && designedRect.height() > 0)
		dirty_rect.setHeight(designedRect.height() - 2*vinset());
	//qfWarning() << "\tdirty_rect 3:" << dirty_rect.toString();
	/// pri rendrovani se muze stat, ze dirtyRect nezacina na bbr, to ale alignment zase spravi
	dirty_rect.moveTopLeft(frame_content_br.topLeft());
	//qfWarning() << "\tdirty_rect:" << dirty_rect.toString();
	//qfDebug() << "\tlayout:" << ((layout == LayoutHorizontal)? "horizontal": "vertical");
	//qfDebug() << "\tortho layout:" << ((orthogonalLayout() == LayoutHorizontal)? "horizontal": "vertical");
	//qfDebug() << "\trenderedRect:" << r.toString();

	/// alignment
	//qfDebug() << "\tALIGN:" << QString::number((int)alignment, 16);
	//alignChildren(mp, dirty_rect);
	//if(0)
	dirty_rect.adjust(-hinset(), -vinset(), hinset(), vinset());
	mp->renderedRect = dirty_rect;
	/// aby sly expandovat deti, musi mit parent spravne renderedRect
	//qfInfo() << "\t rendered rect2:" << mp->renderedRect.toString();
	if(res.value == PrintOk || (res.value == PrintNotFit && (res.flags & FlagPrintBreak))) {
		//bool children_aligned = false;
		/// pokud se vytiskl layout, jehoz vyska nebyla zadana jako % a ma dite s %, roztahni dite a pripadne i jeho deti
		//qfInfo() << childSize(LayoutVertical).fillLayoutRatio();
		if(childSize(LayoutVertical).fillLayoutRatio() < 0) {
			//qfInfo() << "\t expanding";
			if(mp->expandChildVerticalSpringFrames()) {
				/// pokud doslo k expanzi, je treba deti znovu zarovnat
				//qfInfo() << "\t aligning expansion";
				//mp->alignChildren();
				//children_aligned = true;
			}
		}
		//qfInfo() << "\t rendered rect2:" << mp->renderedRect.toString();
		mp->alignChildren();
		mp->renderedRect.flags = designedRect.flags;
		/// mohl bych to udelat tak, ze bych vsem detem dal %, ale je to moc klikani v repeditu
		if(designedRect.flags & ReportItem::Rect::ExpandChildrenFrames) {
			mp->expandChildrenFramesRecursively();
		}
	}
	//dirtyRect = r;//.adjusted(-hinset, -vinset, hinset, vinset);;
	qfDebug() << "\trenderedRect:" << mp->renderedRect.toString();
	res = checkPrintResult(res);
	//qfDebug().color(QFLog::Cyan) << "\t<<<< FRAME return:" << res.toString() << element.tagName() << "id:" << element.attribute("id");
	return res;
}
/*
void ReportItemFrame::alignChildren(ReportItemMetaPaintFrame *mp,  const ReportItem::Rect & dirty_rect)
{
	if(!dirty_rect.isNull()) {
		if(alignment & ~(Qt::AlignLeft | Qt::AlignTop)) {
			Point offset;
			/// ve smeru layoutu posun cely blok
			{
				Rect r1;
				/// vypocitej velikost potisknuteho bloku
				for(int i=0; i<mp->childrenCount(); i++) {
					ReportItemMetaPaint *it = mp->itemAt(i);
					r1 = r1.united(it->renderedRect);
				}
				qreal al = 0, d;
				if(layout == LayoutHorizontal) {
					if(alignment & Qt::AlignHCenter) al = 0.5;
					else if(alignment & Qt::AlignRight) al = 1;
					d = dirty_rect.width() - r1.width();
					if(al > 0 && d > 0)  {
						offset.rx() = d * al - (r1.left() - dirty_rect.left());
					}
				}
				else if(layout == LayoutVertical) {
					if(alignment & Qt::AlignVCenter) al = 0.5;
					else if(alignment & Qt::AlignBottom) al = 1;
					d = dirty_rect.height() - r1.height();
					if(al > 0 && d > 0)  {
						offset.ry() = d * al - (r1.top() - dirty_rect.top());
					}
				}
			}

			/// v orthogonalnim smeru kazdy item
			for(int i=0; i<mp->childrenCount(); i++) {
				ReportItemMetaPaint *it = mp->itemAt(i);
				const Rect &r1 = it->renderedRect;
				qfDebug() << "\t\titem renderedRect:" << r1.toString();
				qreal al = 0, d;

				if(orthogonalLayout() == LayoutHorizontal) {
					offset.rx() = 0;
					if(alignment & Qt::AlignHCenter) al = 0.5;
					else if(alignment & Qt::AlignRight) al = 1;
					d = dirty_rect.width() - r1.width();
					if(al > 0 && d > 0)  {
						qfDebug() << "\t\thorizontal alignment:" << al;
						offset.rx() = d * al - (r1.left() - dirty_rect.left());
					}
				}
				else if(orthogonalLayout() == LayoutVertical) {
					offset.ry() = 0;
					al = 0;
					if(alignment & Qt::AlignVCenter) al = 0.5;
					else if(alignment & Qt::AlignBottom) al = 1;
					d = dirty_rect.height() - r1.height();
					if(al > 0 && d > 0)  {
						qfDebug() << "\t\tvertical alignment:" << al;
						offset.ry() = d * al - (r1.top() - dirty_rect.top());
					}
				}
				qfDebug() << "\t\talign offset:" << offset.toString();
				if(!offset.isNull()) it->shift(offset);
			}
		}
	}
}
*/
/*
ReportItemFrame::Layout ReportItemFrame::parentLayout() const
{
	ReportItemFrame *frm = parentFrame();
	if(!frm) return LayoutInvalid;
	return frm->layout;
}
*/
void ReportItemFrame::resetIndexToPrintRecursively(bool including_para_texts)
{
	//qfInfo() << "resetIndexToPrintRecursively()";
	indexToPrint = 0;
	for(int i=0; i<itemCount(); i++) {
		ReportItem *it = itemAt(i);
		it->resetIndexToPrintRecursively(including_para_texts);
	}
	/*
	foreach(QObject *o, children()) {
		ReportItem *it = qobject_cast<ReportItem*>(o);
		it->resetIndexToPrintRecursively();
	}
	*/
}

//==========================================================
//                   ReportItemReport
//==========================================================
ReportItemReport::ReportItemReport(ReportItem *parent)
	: Super(parent), m_reportProcessor(nullptr)
{
	qfLogFuncFrame();
	//QF_ASSERT(parent, "processor is NULL");
	//Rect r = designedRect;
	//QDomElement el = element.cloneNode(false).toElement();
	//qfDebug() << "\toriginal:" << element.tagName() << "is null:" << element.isNull() << "has children:" << element.hasChildNodes() << "parent node is null:" << element.parentNode().isNull();
	//qfDebug() << "\tclone:" << el.tagName() << "is null:" << el.isNull() << "has children:" << el.hasChildNodes() << "parent node is null:" << el.parentNode().isNull();
	/*--
	if(element.attribute("orientation") == "landscape") {
		Size sz = designedRect.size();
		sz.transpose();
		designedRect.setSize(sz);
	}
	designedRect.flags = (Rect::LeftFixed | Rect::TopFixed | Rect::RightFixed | Rect::BottomFixed);
	--*/
	//--f_dataTable = parent->data();
	//--dataTableLoaded = true;
}

ReportItemReport::~ReportItemReport()
{
	qfLogFuncFrame();
}

ReportItem::PrintResult ReportItemReport::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfLogFuncFrame() << "\x1B[1;31;40m***ROOT***ROOT***ROOT***ROOT***\x1B[0;37;40m" << this;
	Q_UNUSED(bounding_rect);
	PrintResult res = PrintOk;
	//updateChildren();
	//ReportItemMetaPaintPage *pg = new ReportItemMetaPaintPage(out, element, processor()->context());
	//pg->renderedRect = designedRect;
	//indexToPrint = 0; /// vzdy vytiskni header a footer. (footer je absolutni header, umisteny pred detailem)
	res = Super::printMetaPaint(out, designedRect);
	//res = printMetaPaintChildren(pg, pg->renderedRect);
	qfDebug() << "\t\x1B[1;31;40m<<< ***ROOT***ROOT***ROOT***ROOT***\x1B[0;37;40m";
	//res = checkPrintResult(res);
	return res;
}
/*--
qfu::TreeTable ReportItemReport::dataTable()
{
	return f_dataTable;
}
--*/
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
//==========================================================
//                                    ReportItemPara
//==========================================================
ReportItemPara::ReportItemPara(ReportItem * parent)
	: ReportItemFrame(parent)
{
	qfLogFuncFrame();
	//qfInfo() << el.text();
}

void ReportItemPara::resetIndexToPrintRecursively(bool including_para_texts)
{
	if(including_para_texts)
		indexToPrint = 0;
}

ReportItem::PrintResult ReportItemPara::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame();
	return ReportItemFrame::printMetaPaint(out, bounding_rect);
}

ReportItem::PrintResult ReportItemPara::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	PrintResult res = PrintOk;
	if(indexToPrint == 0) {
		printedText = paraText();
	}
	QString text = printedText.mid(indexToPrint);
	int initial_index_to_print = indexToPrint;

	QString sql_id = sqlId();
	/// tiskne se prazdny text
	bool omit_empty_text = isOmitEmptyText();
	if(text.isEmpty() && omit_empty_text) {
	}
	else {
		QString text_to_layout = text;
		//qfWarning() << "length: " << text.length() << " text: [" << text << "]\n" << text.toUtf8().toHex();
		bool text_item_should_be_created = true;
		style::CompiledTextStyle style;
		style::Text *p_text_style = effectiveTextStyle();
		if(p_text_style) {
			style = p_text_style->textStyle();
		}
		/*--
		{
			QString s;
			s = elementAttribute("font");
			if(!!s) style.font = processor()->context().styleCache().font(s);
			s = elementAttribute("pen");
			if(!!s) style.pen = processor()->context().styleCache().pen(s);
			//QBrush brush = processor()->context().brushFromString(element.attribute("brush"));
			qfDebug() << "\tfont:" << style.font.toString();
			//qfDebug() << "\tpen color:" << pen.color().name();
			//qfDebug() << "\tbrush color:" << brush.color().name();
		}
		--*/
		QFontMetricsF font_metrics = processor()->fontMetrics(style.font());
		QTextOption text_option;
		{
			if(isTextWrap())
				text_option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
			//alignment_flags |= Qt::TextWordWrap;
			int al = textHAlign() | textVAlign();
			Qt::Alignment alignment_flags = (Qt::Alignment)al;
			text_option.setAlignment(alignment_flags);
		}
		Rect br;
		/// velikost boundingRect je v mm, tak to prepocitej na body vystupniho zarizeni
		br = qmlwidgets::graphics::mm2device(bounding_rect, processor()->paintDevice());

		bool render_check_mark = false;
		QRegExp rx = ReportItemMetaPaint::checkReportSubstitutionRegExp;
		if(rx.exactMatch(text_to_layout)) {
			//bool check_on = rx.capturedTexts().value(1) == "1";
			br = font_metrics.boundingRect('X');
			render_check_mark = true;
			indexToPrint += text_to_layout.length();
		}
		else {
			if(text_to_layout.isEmpty()) {
				/// neni omitEmptyString, takze i prazdnej text vyrendruj alespon jako mezeru aby se na to dalo treba kliknout
				text_to_layout = ' ';
			}

			//text.replace(ReportItemMetaPaint::checkOnReportSubstitution, "X");
			//text.replace(ReportItemMetaPaint::checkOffReportSubstitution, "X");
			//qfInfo().noSpace().color(QFLog::Green) << "index to print: " << indexToPrint << " text: '" << text << "'";
			//qfInfo() << "bounding rect:" << bounding_rect.toString();
			//qfWarning() << "device physical DPI:" << processor()->paintDevice()->physicalDpiX() << processor()->paintDevice()->physicalDpiY();
			//qfWarning().noSpace() << "'" << text << "' font metrics: " << br.toString();

			//QString text = element.text().simplified().replace("\\n", "\n");
			//qfInfo() << "br:" << br.toString();
			//Rect br_debug = br;
			//bool splitted = false;
			/// do layout
			{
				qreal leading = font_metrics.leading();
				qreal height = 0;
				qreal width = 0;
				textLayout.setFont(style.font());
				//Qt::Alignment alignment = (~Qt::Alignment()) & flags;
				//QTextOption opt(alignment);
				//opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
				textLayout.setTextOption(text_option);
				textLayout.setText(text_to_layout);
				textLayout.beginLayout();
				//bool rubber_frame = designedRect.isRubber(LayoutVertical);
				//int old_pos = 0;
				//QString tx1 = text;
				//qfInfo() << "text to layout:" << text;
				//int line_cnt = 0;
				bool finished = false;
				while (!finished) {
					QTextLine line = textLayout.createLine();
					finished = !line.isValid();
					if(!finished) {
						/*
						if(!finished && line_cnt > 0) {
							qfInfo().noSpace() << "LINE ##: " << line.textStart() << '[' << text.mid(line.textStart(), line.textLength()) << "] + " << line.textLength() << " of " << text.length();
							qfInfo() << "finished:" << finished << "(line.textLength() == 0):" << (line.textLength() == 0) << "(line.textStart() + line.textLength() == text.length()):" << (line.textStart() + line.textLength() == text.length());
						}
						*/
						//line_cnt++;
						//old_pos = line.textStart();
						//qfInfo() << "setting line width to:" << br.width();
						line.setLineWidth(br.width()); /// setWidth() nastavi spravne line.height(), proto musi byt pred merenim popsane vysky.
						//qfInfo() << "text rest:" << text_to_layout.mid(line.textStart());

						if((line.textLength() == 0) && (line.textStart() + line.textLength() == text_to_layout.length())) {
							/// nevim kde je chyba, pri vicerakovych textech mi to pridava jeden prazdnej radek na konec, takhle se tomu snazim zabranit (Qt 4.6.3)
							finished = true;
						}
						else {
							qreal interline_space = (height > 0)? leading: 0;
							if(height + interline_space + line.height() > br.height()) {
								//qfInfo() << "NEEEEEEEE veslo se";
								res = PrintNotFit;
								if(height == 0) {
									/// nevejde se ani jeden radek
									text_item_should_be_created = false;
									break;
								}
								else {
									/// neco se preci jenom veslo
									//splitted = true;
									//qfInfo() << "\tbounding_rect rect:" << bounding_rect.toString();
									//qfInfo() << "\tbr:" << br.toString();
									//qfInfo() << "\theight:" << height;
									int pos = line.textStart();
									indexToPrint += pos;
									//qfInfo() << "POS:" << pos << "index toprint:" << indexToPrint;
									//qfInfo() << text.mid(0, pos).simplified();
									//text_to_layout = text_to_layout.left(pos);
									//res = PrintOk;
									//res.flags = ReportItem::FlagPrintAgain;
									break;
								}
								//line.setLineWidth(123456789); /// vytiskni to az do konce
							}
							height += interline_space;
							//if(line_cnt > 1) qfInfo().noSpace() << "LINE ##: " << line.textStart() << '[' << text.mid(line.textStart(), line.textLength()) << "] + " << line.textLength() << " of " << text.length();
							line.setPosition(QPointF(0., height));
							height += line.height();
							width = qMax(width, line.naturalTextWidth());
						}
					}
					if(finished) {
						//qfInfo() << "veslo se VSECHNO";
						indexToPrint = printedText.length();
						//break;
					}
				}
				textLayout.endLayout();
				br.setWidth(width);
				br.setHeight(height);
				// musim to takhle premerit, jina
				//br = font_metrics.boundingRect(br, flags, text);
				//br = font_metrics.boundingRect(br_debug, 0, text);
				//qfInfo() << "\tbr2:" << br.toString();
			}
		}
		/*
		int x_dpi = processor()->paintDevice()->logicalDpiX();
		int y_dpi = processor()->paintDevice()->logicalDpiY();
		br.setWidth(br.width() * 25.4 / x_dpi);
		br.setHeight(br.height() * 25.4 / y_dpi);
		*/
		/// velikost boundingRect je v bodech vystupniho zarizeni, tak to prepocitej na mm
		br = qmlwidgets::graphics::device2mm(br, processor()->paintDevice());
		/// posun to na zacatek, alignment ramecku to zase vrati
		br.moveTopLeft(bounding_rect.topLeft());
		//qfInfo().noSpace() << "text: '" << text << "'";
		if(text_item_should_be_created ) {
			ReportItemMetaPaintText *mt;
			if(render_check_mark )
				mt = new ReportItemMetaPaintCheck(out, this);
			else {
				mt = new ReportItemMetaPaintText(out, this);
				mt->sqlId = sql_id;
				//--mt->editGrants = elementAttribute("editGrants");
			}
			//qfInfo() << "creating item:" << mt;
			mt->pen = style.pen();
			mt->font = style.font();
			mt->text = text.mid(0, indexToPrint - initial_index_to_print);
			//qfWarning() << "text:" << text;
			mt->textOption = text_option;
			mt->renderedRect = br;
			mt->renderedRect.flags = designedRect.flags;
		}
		//qfDebug().color(QFLog::Green, QFLog::Red) << "\tleading:" << processor()->fontMetrics(style.font).leading() << "\theight:" << processor()->fontMetrics(style.font).height();
		qfDebug() << "\tchild rendered rect:" << br.toString();
	}
	qfDebug() << "\t<<< CHILDREN paraText return:" << res.toString();
	//res = checkPrintResult(res);
	return res;
}

QString ReportItemPara::paraText()
{
	qfLogFuncFrame();
	QString ret = text();
	{
		static QString new_line;
		if(new_line.isEmpty())
			new_line += QChar::LineSeparator;
		ret.replace("\\n", new_line);
		ret.replace("\n", new_line);

		/// jinak nedokazu zadat mezeru mezi dvema <data> elementy nez <data>\s<data>
		ret.replace("\\s", " ");
		/// non breaking space
		ret.replace("\\S", QString(QChar::Nbsp));
	}
	return ret;
}

//==========================================================
//                                    ReportItemBand
//==========================================================
ReportItemBand::ReportItemBand(ReportItem *parent)
	: ReportItemFrame(parent), dataTableLoaded(false)
{
	qfLogFuncFrame();
}

ReportItemBand::~ReportItemBand()
{
	qfLogFuncFrame();
}

void ReportItemBand::resetIndexToPrintRecursively(bool including_para_texts)
{
	ReportItemFrame::resetIndexToPrintRecursively(including_para_texts);
	dataTableLoaded = false;
	ReportItemDetail *det = detail();
	if(det) {
		//qfInfo() << "resetCurrentRowNo() elid:" << det->element.attribute("id");
		det->resetCurrentRowNo();
	}
}

ReportItemDetail* ReportItemBand::detail()
{
	ReportItemDetail *ret = NULL;
	for(int i=0; i<itemCount(); i++) {
		ReportItem *it = itemAt(i);
		ret = it->toDetail();
		if(ret) break;
	}
	return ret;
}

qfu::TreeTable ReportItemBand::dataTable()
{
	//qfLogFuncFrame() << "dataTableLoaded:" << dataTableLoaded;
	if(!dataTableLoaded) {
		f_dataTable = data();
		if(f_dataTable.isNull()) {
			QString data_src = dataSource();
			f_dataTable = findDataTable(data_src);
		}
		//qfu::TreeTable t =v.value<qfu::TreeTable>();
		dataTableLoaded = true;
	}
	return f_dataTable;
}

ReportItem::PrintResult ReportItemBand::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << dataTable().toString();
	/*--
	if(dataTable().isNull() && !processor()->isDesignMode()) { /// pokud neni table (treba bez radku), band se vubec netiskne
		PrintResult res;
		res.value = PrintOk;
		return res;
	}
	--*/
	if(isHeaderOnBreak()) {
		/// print everything except of detail again
		for(int i=0; i<itemCount(); i++) {
			ReportItem *it = itemAt(i);
			if(it->toDetail() == NULL)
				it->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
		}
		indexToPrint = 0;
	}
	PrintResult res = ReportItemFrame::printMetaPaint(out, bounding_rect);
	qfDebug() << "\tRETURN:" << res.toString();
	return res;
}

//==========================================================
//                                    ReportItemDetail
//==========================================================
ReportItemDetail::ReportItemDetail(ReportItem *parent)
	: ReportItemFrame(parent)
{
	//qfInfo() << QF_FUNC_NAME << "element id:" << element.attribute("id");
	f_currentRowNo = -1;
}

qfu::TreeTable ReportItemDetail::dataTable()
{
	ReportItemBand *b = parentBand();
	qfu::TreeTable data_table;
	if(b) {
		//qfDebug() << "band:" << b << "\ttable is null:" << b->dataTable().isNull();
		data_table = b->dataTable();
	}
	return data_table;
}

qfu::TreeTableRow ReportItemDetail::dataRow()
{
	return dataTable().row(currentRowNo());
}

void ReportItemDetail::resetIndexToPrintRecursively(bool including_para_texts)
{
	ReportItemFrame::resetIndexToPrintRecursively(including_para_texts);
}

ReportItem::PrintResult ReportItemDetail::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << QF_FUNC_NAME;
	bool design_mode = processor()->isDesignMode(); /// true znamena, zobraz prvni radek, i kdyz tam nejsou data.
	//qfInfo() << "design mode:" << design_mode;
	ReportItemBand *b = parentBand();
	qfu::TreeTable data_table;
	if(b) {
		data_table = b->dataTable();
		//qfInfo()<< element.attribute("id") << "band:" << b << "\ttable is null:" << b->dataTable().isNull() << "f_currentRowNo:" << f_currentRowNo << "of" << data_table.rowCount();
		if(!data_table.isNull()) {
			//design_view = false;
			if(currentRowNo() < 0) {
				/// kdyz neni f_dataRow, vezmi prvni radek dat
				//qfInfo() << "init f_currentRowNo to 0, element id:" << element.attribute("id");
				resetCurrentRowNo();
			}
		}
	}
	PrintResult res;
	if(!design_mode && (data_table.isNull() || dataRow().isNull())) {
		/// prazdnej detail vubec netiskni
		res.value = PrintOk;
		return res;
	}
	res = ReportItemFrame::printMetaPaint(out, bounding_rect);
	if(res.value == PrintOk) {
		if(b) {
			/// vezmi dalsi radek dat
			f_currentRowNo++;
			//qfInfo() << "vezmi dalsi radek dat element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
			if(currentRowNo() < data_table.rowCount()) {
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				res.flags |= FlagPrintAgain;
			}
			else {
				//qfInfo() << "detail setting f_currentRowNo to 0, element id:" << element.attribute("id");
				resetCurrentRowNo();
			}
		}
	}
	else {
		//qfWarning() << "detail print !OK, element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
	}
	//res = checkPrintResult(res);
	qfDebug() << "\treturn:" << res.toString();
	return res;
}
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
/*
//===============================================================
//                                               ReportItemIf
//===============================================================
ReportItem::PrintResult ReportItemIf::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfDebug().color(QFLog::Magenta) << QF_FUNC_NAME << element.tagName() << "id:" << element.attribute("id");
	qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	PrintResult res = PrintOk;
	Rect bbr = bounding_rect;

	QDomElement el = element.firstChildElement();
	if(!!el) {
		bool bool_res = nodeValue(el).toBool();
		ReportItem *it_res = NULL;
		for(int i=indexToPrint; i<itemCount(); i++) {
			ReportItem *it = childAt(i);
			if(bool_res) {
				if(qobject_cast<ReportItemIfTrue*>(it)) {
					it_res = it;
					break;
				}
			}
			else {
				if(qobject_cast<ReportItemIfFalse*>(it)) {
					it_res = it;
					break;
				}
			}
		}
		if(it_res) {
			res = it_res->printMetaPaint(out, bbr);
		}
	}
	return res;
}
*/
//===============================================================
//                ReportItemImage
//===============================================================
/*--
bool ReportItemImage::childrenSynced()
{
	return childrenSyncedFlag;
}

void ReportItemImage::syncChildren()
{
	//qfDebug() << QF_FUNC_NAME << element.tagName() << "id:" << element.attribute("id");
	QString orig_src = element.attribute("src");
	QString processor_img_key;
	src = orig_src;
	ReportItem::Image im;
	if(orig_src.startsWith("key:/")) {
		/// obrazek je ocekavan v processor()->images(), takze neni treba delat nic
		src = QString();
	}
	else if(orig_src.isEmpty()) {
		/// obrazek bude v datech
		src = QString();
	}
	if(!src.isEmpty()) {
		if(src.startsWith("./") || src.startsWith("../")) {
			src = QFFileUtils::joinPath(QFFileUtils::path(processor()->report().fileName()), src);
			//qfInfo() << "relative path joined to:" << src;
		}
		src = processor()->searchDirs()->findFile(src);
		if(src.isEmpty()) {
			qfWarning().noSpace() << "file '" << orig_src << "' not found. Report file name: " << processor()->report().fileName();
			/// pridej fake para element, aby se jmeno chybejiciho souboru zobrazilo v reportu
			if(fakeLoadErrorPara.isNull()) {
				//qfInfo() << "creating fakeLoadErrorPara:" << orig_src;
				fakeLoadErrorPara = fakeLoadErrorParaDocument.createElement("para");
				fakeLoadErrorParaDocument.appendChild(fakeLoadErrorPara);
				fakeLoadErrorPara.setAttribute("__fake", 1);
				fakeLoadErrorPara.appendChild(fakeLoadErrorPara.ownerDocument().createTextNode(orig_src));
				processor()->createProcessibleItem(fakeLoadErrorPara, this);
				//qfInfo() << "children cnt:" << this->itemCount();
				//qfInfo() << "this:" << this << "itemCount" << itemCount() << "\n" << toString();
			}
		}
	}

	qfDebug() << "orig_src:" << orig_src;
	qfDebug() << "src:" << src;
	if(!src.isEmpty()) {
		if(src.endsWith(".svg", Qt::CaseInsensitive)) {
			QSvgRenderer ren;
			if(!ren.load(processor()->searchDirs()->loadFile(src))) qfWarning() << "SVG data read error src:" << src;
			else {
				//qfInfo() << "default size::" << ren.defaultSize().width() << ren.defaultSize().height();
				QPicture pic;
				QPainter painter(&pic);
				ren.render(&painter);
				painter.end();
				im.picture = pic;
				//qfInfo() << "bounding rect:" << Rect(pic.boundingRect()).toString();
			}
		}
		else {
			QByteArray ba = processor()->searchDirs()->loadFile(src);
			if(!im.image.loadFromData(ba)) {
				qfWarning() << "ERROR load image:" << src;
			}
			//im.image = QImage(src);
		}
		if(!im.isNull()) processor_img_key = orig_src;
	}
	if(im.isNull() && !orig_src.isEmpty()) {
		/// pridej fake para element, aby se jmeno chybejiciho souboru zobrazilo v reportu
		qfWarning().noSpace() << "QImage('" << src << "') constructor error.";
		if(fakeLoadErrorPara.isNull()) {
			fakeLoadErrorPara = fakeLoadErrorParaDocument.createElement("para");
			fakeLoadErrorParaDocument.appendChild(fakeLoadErrorPara);
			fakeLoadErrorPara.setAttribute("__fake", 1);
			fakeLoadErrorPara.appendChild(fakeLoadErrorPara.ownerDocument().createTextNode("QImage('" + src + "') constructor error."));
			processor()->createProcessibleItem(fakeLoadErrorPara, this);
		}
		src = QString();
	}
	else {
		processor()->addImage(processor_img_key, ReportItem::Image(im));
		src = processor_img_key;
	}
	qfDebug() << "src:" << src;
	//ReportItem::syncChildren();
	childrenSyncedFlag = true;
}
--*/
ReportItemImage::ReportItemImage(ReportItem *parent)
	: Super(parent)
{
	connect(this, &ReportItemImage::dataSourceChanged, this, &ReportItemImage::updateResolvedDataSource);
}

void ReportItemImage::updateResolvedDataSource(const QString &data_source)
{
	QString processor_img_key;
	m_resolvedDataSource = data_source;
	ReportItem::Image im;
	if(data_source.startsWith("key:/")) {
		/// obrazek je ocekavan v processor()->images(), takze neni treba delat nic
	}
	else if(data_source.isEmpty()) {
		/// obrazek bude v datech
		m_resolvedDataSource = QString();
	}
	if(!m_resolvedDataSource.isEmpty()) {
		if(m_resolvedDataSource.startsWith("./") || m_resolvedDataSource.startsWith("../")) {
			m_resolvedDataSource = qfu::FileUtils::joinPath(qfu::FileUtils::path(processor()->reportUrl().toLocalFile()), m_resolvedDataSource);
			//qfInfo() << "relative path joined to:" << m_resolvedDataSource;
		}
		/*--
		m_resolvedDataSource = processor()->searchDirs()->findFile(m_resolvedDataSource);
		if(m_resolvedDataSource.isEmpty()) {
			qfWarning().noSpace() << "file '" << data_source << "' not found. Report file name: " << processor()->report().fileName();
			/// pridej fake para element, aby se jmeno chybejiciho souboru zobrazilo v reportu
			if(fakeLoadErrorPara.isNull()) {
				//qfInfo() << "creating fakeLoadErrorPara:" << data_source;
				fakeLoadErrorPara = fakeLoadErrorParaDocument.createElement("para");
				fakeLoadErrorParaDocument.appendChild(fakeLoadErrorPara);
				fakeLoadErrorPara.setAttribute("__fake", 1);
				fakeLoadErrorPara.appendChild(fakeLoadErrorPara.ownerDocument().createTextNode(data_source));
				processor()->createProcessibleItem(fakeLoadErrorPara, this);
				//qfInfo() << "children cnt:" << this->itemCount();
				//qfInfo() << "this:" << this << "itemCount" << itemCount() << "\n" << toString();
			}
		}
		--*/
	}

	qfDebug() << "data_source:" << data_source;
	qfDebug() << "m_resolvedDataSource:" << m_resolvedDataSource;
	if(!m_resolvedDataSource.isEmpty()) {
		if(m_resolvedDataSource.endsWith(".svg", Qt::CaseInsensitive)) {
			QSvgRenderer ren;
			if(!ren.load(m_resolvedDataSource)) {
				qfWarning() << "SVG data read error m_resolvedDataSource:" << m_resolvedDataSource;
			}
			else {
				//qfInfo() << "default size::" << ren.defaultSize().width() << ren.defaultSize().height();
				QPicture pic;
				QPainter painter(&pic);
				ren.render(&painter);
				painter.end();
				im.picture = pic;
				//qfInfo() << "bounding rect:" << Rect(pic.boundingRect()).toString();
			}
		}
		else {
			if(!im.image.load(m_resolvedDataSource)) {
				qfWarning() << "ERROR load image:" << m_resolvedDataSource;
			}
		}
		if(!im.isNull())
			processor_img_key = data_source;
	}
	if(im.isNull() && !data_source.isEmpty()) {
		/// pridej fake para element, aby se jmeno chybejiciho souboru zobrazilo v reportu
		qfWarning().nospace() << "QImage('" << m_resolvedDataSource << "') constructor error.";
		/*--
		if(fakeLoadErrorPara.isNull()) {
			fakeLoadErrorPara = fakeLoadErrorParaDocument.createElement("para");
			fakeLoadErrorParaDocument.appendChild(fakeLoadErrorPara);
			fakeLoadErrorPara.setAttribute("__fake", 1);
			fakeLoadErrorPara.appendChild(fakeLoadErrorPara.ownerDocument().createTextNode("QImage('" + m_resolvedDataSource + "') constructor error."));
			processor()->createProcessibleItem(fakeLoadErrorPara, this);
		}
		--*/
		m_resolvedDataSource = QString();
	}
	else {
		processor()->addImage(processor_img_key, ReportItem::Image(im));
		m_resolvedDataSource = processor_img_key;
	}
	qfDebug() << "m_resolvedDataSource:" << m_resolvedDataSource;
	//ReportItem::syncChildren();
	//--childrenSyncedFlag = true;
}

ReportItem::PrintResult ReportItemImage::printMetaPaint(ReportItemMetaPaint* out, const ReportItem::Rect& bounding_rect)
{
	ReportItem::PrintResult ret = ReportItemFrame::printMetaPaint(out, bounding_rect);
	/*--
	ReportItemMetaPaint *mpi = out->lastChild();
	if(mpi) {
		ReportItemMetaPaintImage *img = qobject_cast<ReportItemMetaPaintImage*>(mpi->lastChild());
		if(img) {
			/// pokud se obrazek vytiskl a je to background, nastav tento flag jeho rodicovskemu frame
			if(QString(elementAttribute("backgroundItem")).toBool()) {
				mpi->renderedRect.flags |= Rect::BackgroundItem;
				//img->renderedRect.setWidth(0);
				//img->renderedRect.setHeight(0);
				//qfWarning() << mpi << "setting backgroundItem" << (mpi->renderedRect.flags & Rect::BackgroundItem);
			}
			//qfInfo() << elementAttribute("backgroundItem") << "fs:" << QString(elementAttribute("backgroundItem")).toBool() << "rendered rect:" << img->renderedRect.toString() << "image size:" << im.size().width() << "x" << im.size().height();
		}
	}
	--*/
	return ret;
}

ReportItemImage::PrintResult ReportItemImage::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	//qfDebug().color(QFLog::Magenta) << QF_FUNC_NAME << element.tagName() << "id:" << element.attribute("id");
	//qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	PrintResult res = PrintOk;
	Rect br = bounding_rect;
	/*--
	if(!fakeLoadErrorPara.isNull()) {
		/// src nebyl nalezen, child je para, kde je uvedeno, jak se jmenoval nenalezeny obrazek
		//qfInfo() << "this:" << this << "itemCount" << itemCount() << "\n" << toString();
		ReportItemFrame::printMetaPaintChildren(out, bounding_rect);
	}
	else
	--*/
	{
		QString src = dataSource();
		ReportItem::Image im = processor()->images().value(src);
		if(src.isEmpty()) {
			/// muze byt jeste v datech, zkus ho nahrat pro aktualni radek
			QString data_s = data();
			//QString data_s = nodeText(el);
			//qfError() << data_s;
			QByteArray img_data;
			{
				DataEncoding encoding = dataEncoding();
				if(encoding == EncodingBase64) {
					img_data = QByteArray::fromBase64(data_s.toLatin1());
				}
				else if(encoding == EncodingHex) {
					img_data = QByteArray::fromHex(data_s.toLatin1());
				}
				else {
					qfError() << "Need dataEncoding property filled to decode data.";
				}
			}
			{
				DataCompression compression = dataCompression();
				if(compression == CompressionQCompress) {
					img_data = qUncompress(img_data);
				}
			}
			if(!img_data.isEmpty()) {
				DataFormat format = dataFormat();
				if(format == FormatSvg) {
					QSvgRenderer ren;
					if(!ren.load(img_data)) {
						qfWarning() << "SVG data read error, format:" << format;
					}
					else {
						QPicture pic;
						QPainter painter(&pic);
						ren.render(&painter);
						painter.end();
						im.picture = pic;
					}
				}
				else if(format == FormatQPicture) {
					QBuffer buff(&img_data);
					QPicture pic;
					buff.open(QIODevice::ReadOnly);
					pic.load(&buff);
					//qfInfo() << "PIC size:" << Rect(pic.boundingRect()).toString();
					im.picture = pic;
				}
				else {
					const char *fmt = nullptr;
					if(format == FormatJpg)
						fmt = "jpg";
					else if(format == FormatPng)
						fmt = "png";
					if(!im.image.loadFromData(img_data, fmt))
						qfWarning() << "Image data read error, format:" << format;
				}
			}
		}
		if(im.isNull())
			qfWarning() << "Printing an empty image";
		ReportItemMetaPaintImage *img = new ReportItemMetaPaintImage(out, this);
		img->setSuppressPrintOut(isSuppressPrintout());
		//qfInfo() << "\t src:" << src;
		//qfInfo() << "\t processor()->images().contains(" << src << "):" << processor()->images().contains(src);
		img->aspectRatioMode = (Qt::AspectRatioMode)aspectRatio();
		/// vymysli rozmer br, do kteryho to potom reportpainter nacpe, at je to veliky jak chce
		if(designedRect.width() == 0 && designedRect.horizontalUnit == Rect::UnitMM && designedRect.height() == 0 && designedRect.verticalUnit == Rect::UnitMM) {
			/// ani jeden smer neni zadan, vezmi ozmery z obrazku
			double w = 0;
			double h = 0;
			if(im.isImage()) {
				w = im.image.width() / (im.image.dotsPerMeterX() / 1000.);
				h = im.image.height() / (im.image.dotsPerMeterY() / 1000.);
				if(w > 0 && w < br.width()) br.setWidth(w);
				if(h > 0 && h < br.height()) br.setHeight(w);
				//qfInfo() << "image bounding rect w:" << w << "h:" << h << "designed rect:" << designedRect.toString();
			}
			else if(im.isPicture()) {
				w  =im.picture.boundingRect().width();
				h = im.picture.boundingRect().height();
				//qfInfo() << "picture bounding rect w:" << w << "h:" << h << "designed rect:" << designedRect.toString();
			}
		}
		else if(designedRect.width() == 0 && designedRect.horizontalUnit == Rect::UnitMM) {
			/// rubber ve smeru x
			Size sz = im.size();
			//qfInfo() << "br0:" << bounding_rect.toString();
			//qfInfo() << "image size:" << sz.toString();
			{
				br = bounding_rect;
				Size br_sz = br.size();
				br_sz.setHeight(designedRect.height());
				sz.scale(br_sz, img->aspectRatioMode);
				br.setSize(sz);
				//qfInfo() << "br2:" << br.toString();
			}
		}
		else if(designedRect.height() == 0 && designedRect.verticalUnit == Rect::UnitMM) {
			/// rubber ve smeru y
			Size sz = im.size();
			br = bounding_rect;
			Size br_sz = br.size();
			br_sz.setWidth(designedRect.width());
			sz.scale(br_sz, img->aspectRatioMode);
			br.setSize(sz);
		}
		else {
			/// oba smery zadany
			Size sz = im.size();
			br = bounding_rect;
			Size br_sz = br.size();
			sz.scale(br_sz, img->aspectRatioMode);
			br.setSize(sz);
		}
		img->image = im;
		img->renderedRect = br;
		img->renderedRect.flags = designedRect.flags;
	}

	return res;
}

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
