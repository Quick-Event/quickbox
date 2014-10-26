#include "reportprocessor.h"
#include "reportitemreport.h"
#include "reportpainter.h"
#include "../../framework/application.h"

#include <qf/core/utils/fileutils.h>
#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QSet>
#include <QJsonDocument>
#include <QQmlEngine>
#include <QDomElement>

namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//===================================================
//                                ReportProcessor
//===================================================
ReportProcessor::ReportProcessor(QPaintDevice *paint_device, QObject *parent)
	: QObject(parent)//, m_Context(qf::qmlwidgets::graphics::StyleCache())
{
	qfLogFuncFrame();
	m_qmlEngine = nullptr;
	//--f_searchDirs = search_dirs;
	m_paintDevice = paint_device;
	m_processedPageNo = 0;
	m_designMode = false;
}

ReportProcessor::~ReportProcessor()
{
	qfLogFuncFrame() << this << "m_documentInstanceRoot:" << m_documentInstanceRoot;
	QF_SAFE_DELETE(m_documentInstanceRoot);
	//qfInfo() << "delete ReportProcessor" << this;
}

/*--
qfu::SearchDirs* ReportProcessor::searchDirs(bool throw_exc)
{
	if(!f_searchDirs && throw_exc) QF_EXCEPTION(trUtf8("ReportProcessor search dirs is NULL."));
	return f_searchDirs;
}
--*/
void ReportProcessor::reset()
{
	qfLogFuncFrame();
	QF_SAFE_DELETE(m_documentInstanceRoot);
	QF_SAFE_DELETE(m_processorOutput);
}

/*--
void ReportProcessor::setReport(const ReportDocument &doc)
{
	qfLogFuncFrame();
	fReport = doc;
	reset();
}
--*/
void ReportProcessor::setReport(const QString &rep_file_name)
{
	QF_SAFE_DELETE(m_reportDocumentComponent);
	QString fn = rep_file_name;
	m_reportDocumentComponent = new ReportDocument(qmlEngine(true), this);
	m_reportDocumentComponent->setFileName(rep_file_name);
	if(m_reportDocumentComponent->isError()) {
		qfError() << "Erorr loading report component:" << m_reportDocumentComponent->errorString();
		return;
	}
	if(!m_reportDocumentComponent->isReady()) {
		qfError() << "QML component" << m_reportDocumentComponent->url().toString() << "cannot be loaded asynchronously";
		return;
	}
}

QUrl ReportProcessor::reportUrl() const
{
	QUrl ret;
	if(m_reportDocumentComponent) {
		ret = m_reportDocumentComponent->url();
	}
	return ret;
}

void ReportProcessor::setData(const qfu::TreeTable &_data)
{
	//qfInfo() << "ReportProcessor _data:" << _data.toString().mid(0, 100);
	m_data = _data;
	//qfInfo() << "SET reportProcessor data:" << m_data.toString();
}

ReportItemReport* ReportProcessor::documentInstanceRoot()
{
	if(!m_documentInstanceRoot) {
		QObject *o = qobject_cast<ReportItemReport*>(m_reportDocumentComponent->create(qmlEngine()->rootContext()));
		m_documentInstanceRoot = qobject_cast<ReportItemReport*>(o);
		if(!m_documentInstanceRoot) {
			qfError() << "Error creating root object from component:" << m_reportDocumentComponent;
			qfError() << "Created object:" << o;
			QF_SAFE_DELETE(o);
		}
		else {
			m_documentInstanceRoot->setReportProcessor(this);
			style::Text *st = m_documentInstanceRoot->textStyle();
			if(!st) {
				qfWarning() << "Report document has not the textStyle property set to valid TextStyle object, "
							   "Para.text will not have implicit TextStyle definition.";
			/*
				st = new style::Text(m_documentInstanceRoot);
				st->setProperty("basedOn", "default");
				m_documentInstanceRoot->setTextStyle(st);
			*/
			}
			style::Sheet *ss = m_documentInstanceRoot->styleSheet();
			if(ss)
				ss->createStyleCache();
			setDesignMode(m_documentInstanceRoot->debugLevel() > 0);
		}
	}
	return m_documentInstanceRoot;
}

void ReportProcessor::process(ReportProcessor::ProcessorMode mode)
{
	qfLogFuncFrame() << "mode:" << mode;
	if(mode == FirstPage || mode == AllPages) {
		m_processedPageNo = 0;
		QF_SAFE_DELETE(m_processorOutput);
		if(documentInstanceRoot()) {
			m_processorOutput = new ReportItemMetaPaintReport(documentInstanceRoot());
			m_singlePageProcessResult = ReportItem::PrintResult(ReportItem::PrintNotFit);
		}
	}
	ReportItemMetaPaint mpit;
	//context().dump();
	while(m_singlePageProcessResult.value == ReportItem::PrintNotFit
		  && !(m_singlePageProcessResult.flags & ReportItem::FlagPrintNeverFit)) {
		m_singlePageProcessResult = processPage(&mpit);
		qfDebug() << "singlePageProcessResult:" << m_singlePageProcessResult.toString();
		//qfDebug().color(QFLog::Yellow) << context().styleCache().toString();
		//mpit.dump();
		ReportItemMetaPaint *it = mpit.firstChild();
		if(it) {
			it->setParent(m_processorOutput);
			if(mode == FirstPage || mode == SinglePage) {
				emit pageProcessed();
				if(m_singlePageProcessResult.value == ReportItem::PrintNotFit) {
					m_processedPageNo++;
				}
				//qfInfo() << "pageProcessed:" << fProcessedPageNo;
				break;
			}
			else {
				if(m_singlePageProcessResult.value == ReportItem::PrintNotFit) {
					m_processedPageNo++;
				}
				else {
					break;
				}
			}

		}
		else break;
	}
}

ReportItem::PrintResult ReportProcessor::processPage(ReportItemMetaPaint *out)
{
	qfLogFuncFrame();
	ReportItem::PrintResult res;
	if(documentInstanceRoot()) {
		res = documentInstanceRoot()->printMetaPaint(out, ReportItem::Rect());
		qfDebug() << "\tres:" << res.toString();
	}
	return res;
}

QFontMetricsF ReportProcessor::fontMetrics(const QFont &font)
{
	return QFontMetricsF(font, paintDevice());
}
/*
QString ReportProcessor::resolveFileName(const QString &f_name)
{
	qfLogFuncFrame();
	QString ret = f_searchDirs.findFile(f_name);
	if(ret.isEmpty()) QF_EXCEPTION(tr("File '%1' can not be resolved trying %2.").arg(f_name).arg(f_searchDirs.dirs().join(", ")));
	return ret;
}
*/
/*
QString ReportProcessor::resolveFN(const QString &f_name)
{
	return resolveFileName(f_name);
}
*/

void ReportProcessor::processHtml(QDomElement & el_body)
{
	documentInstanceRoot()->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
	documentInstanceRoot()->printHtml(el_body);
	fixTableTags(el_body);
	removeRedundantDivs(el_body);
	/// tak a z divu s horizontalnim layoutem udelej tabulky
	fixLayoutHtml(el_body);
}

void ReportProcessor::fixTableTags(QDomElement & _el)
{
	QDomElement el(_el);
	bool is_table_row = false;
	bool is_table_header_row = false;
	QString attr = el.attribute("__table");
	if(!attr.isEmpty()) {
		attr = attr.mid(QString("__fakeBand").length());
		if(attr == "Table") {
			el.setTagName("table");
			el.setAttribute("border", 1);
		}
		else if(attr == "Detail") {el.setTagName("tr"); is_table_row = true;}
		else if(attr == "HeaderRow") {el.setTagName("tr"); is_table_header_row = true;}
		else if(attr == "FooterRow") {el.setTagName("tr"); is_table_header_row = true;}
	}
	for(QDomElement el1 = el.firstChildElement(); !el1.isNull(); el1 = el1.nextSiblingElement()) {
		fixTableTags(el1);
		if(is_table_row) el1.setTagName("td");
		if(is_table_header_row) el1.setTagName("th");
	}
}

QDomElement ReportProcessor::removeRedundantDivs(QDomElement & _el)
{
	qfLogFuncFrame() << _el.tagName() << "children cnt:" << _el.childNodes().count();
	QDomElement el(_el);
	//qfDebug() << "\t path:" << el.path();
	/// pokud ma div prave jedno dite, je na prd
	while(el.tagName() == "div" && el.childNodes().count() == 1) {
		QDomNode parent_nd = el.parentNode();
		QDomElement el_child = el.childNodes().at(0).toElement();
		if(el_child.isNull()) break;
		if(!el_child.isNull() && !parent_nd.isNull()) {
			qfDebug() << "\t child el:" << el_child.tagName() << "children cnt:" << el_child.childNodes().count();
			parent_nd.replaceChild(el_child, el).toElement();
			el = el_child;
			qfDebug() << "\t new el:" << el.tagName() << "children cnt:" << el.childNodes().count();
		}
	}
	qfDebug() << "\t checking children of el:" << el.tagName() << "children cnt:" << el.childNodes().count();
	for(QDomElement el1 = el.firstChildElement(); !el1.isNull(); el1 = el1.nextSiblingElement()) {
		el1 = removeRedundantDivs(el1);
	}
	return el;
}

QDomElement ReportProcessor::fixLayoutHtml(QDomElement & _el)
{
	qfLogFuncFrame() << _el.tagName() << "children cnt:" << _el.childNodes().count();
	QDomElement el(_el);
	if(el.tagName() == "div") {
		QString attr = el.attribute("layout");
		if(attr == "horizontal") {
			QDomNode parent_nd = el.parentNode();
			if(!parent_nd.isNull()) {
				QDomElement el_table = el.ownerDocument().createElement("table").toElement();
				QDomNode old_el = parent_nd.replaceChild(el_table, el);
				QDomNode el_tr = el.ownerDocument().createElement("tr");
				el_table.appendChild(el_tr);
				while(true) {
					QDomElement el1 = old_el.firstChildElement();
					if(el1.isNull()) break;
					QDomNode el_td = el.ownerDocument().createElement("td");
					el_tr.appendChild(el_td);
					el_td.appendChild(el1);
				}
				el = el_table;
			}
		}
	}
	for(QDomElement el1 = el.firstChildElement(); !el1.isNull(); el1 = el1.nextSiblingElement()) {
		el1 = fixLayoutHtml(el1);
	}
	return el;
}

QQmlEngine *ReportProcessor::qmlEngine(bool throw_exc)
{
#if defined USE_APP_ENGINE
	QQmlEngine *ret = nullptr;
	qf::qmlwidgets::framework::Application *app = qobject_cast<qf::qmlwidgets::framework::Application*>(QCoreApplication::instance());
	if(throw_exc)
		QF_ASSERT_EX(app != nullptr, "Application is not a type of qf::qmlwidgets::framework::Application");
	if(app) {
		ret = app->qmlEngine();
		if(throw_exc)
			QF_ASSERT_EX(ret != nullptr, "Application has not QML engine created.");
	}
	return ret;
#else
	Q_UNUSED(throw_exc);
	if(!m_qmlEngine) {
		m_qmlEngine = new QQmlEngine(this);
		QString path;
#ifdef Q_OS_UNIX
		path = QCoreApplication::applicationDirPath() + "/../lib/qml";
#else
		path = QCoreApplication::applicationDirPath() + "/qml";
#endif
		m_qmlEngine->addImportPath(path);
	}
	return m_qmlEngine;
#endif
}

void ReportProcessor::dump()
{
	if(m_processorOutput) m_processorOutput->dump();
}

void ReportProcessor::print(QPrinter &printer, const QVariantMap &options)
{
	qfLogFuncFrame();

	ReportPainter painter(&printer);

	typedef ReportItem::Rect Rect;
	//typedef ReportItem::Size Size;

	int pg_no = options.value("fromPage", 1).toInt() - 1;
	int to_page = options.value("toPage", pageCount()).toInt();
	qfDebug() << "pg_no:" << pg_no << "to_page:" << to_page;
	ReportItemMetaPaintFrame *frm = getPage(pg_no);
	if(frm) {
		Rect r = frm->renderedRect;
		bool landscape = r.width() > r.height();
		if(landscape) printer.setOrientation(QPrinter::Landscape);
		//Rect printer_pg_rect = QRectF(printer.pageRect());
		//qfWarning() << "\tprinter page rect:" << printer_pg_rect.toString();
		//qfWarning() << "\tresolution:" << printer.resolution() << Size(printer_pg_rect.size()/printer.resolution()).toString(); /// resolution je v DPI
		//qreal magnify = printer_pg_rect.width() / r.width();
		//painter.scale(magnify, magnify);
		painter.pageCount = pageCount();
		while(frm) {
			//painter.currentPage = pg_no;
			painter.drawMetaPaint(frm);
			pg_no++;
			frm = getPage(pg_no);
			if(!frm) break;
			if(pg_no >= to_page) break;
			printer.newPage();
		}
	}
}

ReportItemMetaPaintFrame* ReportProcessor::getPage(int n)
{
	if(!processorOutput()) return NULL;
	if(n < 0 || n >= processorOutput()->childrenCount()) return NULL;
	ReportItemMetaPaint *it = processorOutput()->child(n);
	ReportItemMetaPaintFrame *frm	= dynamic_cast<ReportItemMetaPaintFrame*>(it);
	qfDebug() << "\treturn:" << frm;
	return frm;
}

int ReportProcessor::pageCount()
{
	qfLogFuncFrame();
	int ret = 0;
	if(processorOutput()) {
		ret = processorOutput()->childrenCount();
	}
	return ret;
}
