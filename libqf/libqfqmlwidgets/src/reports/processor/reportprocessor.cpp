#include "reportitem.h"
#include "reportprocessor.h"
#include "reportpainter.h"
#include "../../framework/application.h"

#include <qf/core/utils/fileutils.h>
#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QSet>
#include <QJsonDocument>
#include <QQmlEngine>

namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//===================================================
//                                ReportProcessor
//===================================================
ReportProcessor::ReportProcessor(QPaintDevice *paint_device, QObject *parent)
	: QObject(parent), f_Context(qf::qmlwidgets::graphics::StyleCache()), m_documentInstanceRoot(NULL), f_processorOutput(NULL)
{
	//qfInfo() << "new ReportProcessor" << this;
	m_qmlEngine = nullptr;
	//--f_searchDirs = search_dirs;
	fPaintDevice = paint_device;
	fProcessedPageNo = 0;
	setDesignMode(false);
}

ReportProcessor::~ReportProcessor()
{
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
	makeContext();
	QF_SAFE_DELETE(m_documentInstanceRoot);
	QF_SAFE_DELETE(f_processorOutput);
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
	m_reportDocumentComponent = new ReportDocument(qmlEngine(true), rep_file_name, this);
	if(m_reportDocumentComponent->isError()) {
		qfError() << "Erorr loading report component:" << m_reportDocumentComponent->errorString();
		return;
	}
	if(!m_reportDocumentComponent->isReady()) {
		qfError() << "QML component" << rep_file_name << "cannot be loaded asynchronously";
		return;
	}
}

void ReportProcessor::setData(const qfu::TreeTable &_data)
{
	//qfInfo() << "ReportProcessor _data:" << _data.toString().mid(0, 100);
	f_data = _data;
	//qfInfo() << "ReportProcessor data:" << fData.toString().mid(0, 100);
}

void ReportProcessor::makeContext()
{
	qfLogFuncFrame();
	contextRef().clear(); /// nemuzu tady dat contexRef() = ReportProcessorContext(), protoze ten ma globalni cache, to je tim, ze qf::qmlwidgets::graphics::StyleCache je explicitne sdilena, a ja potrebuju, pro kazdou instanci report processoru vlastni
	qfError() << "NIY";
	QObject *stylesheet_obj = nullptr;
	// TODO: find stylesheet
	contextRef().styleCacheRef().readStyleSheet(stylesheet_obj);
}

ReportItemReport* ReportProcessor::documentInstanceRoot()
{
	if(!m_documentInstanceRoot) {
		m_documentInstanceRoot = m_reportDocumentComponent->create(qmlEngine()->rootContext());
	}
	return m_documentInstanceRoot;
}

void ReportProcessor::process(ReportProcessor::ProcessorMode mode)
{
	qfLogFuncFrame() << "mode:" << mode;
	if(mode == FirstPage || mode == AllPages) {
		fProcessedPageNo = 0;
		SAFE_DELETE(f_processorOutput);
		if(processedItemsRoot()) {
			f_processorOutput = new ReportItemMetaPaintReport(processedItemsRoot());
			singlePageProcessResult = ReportProcessorItem::PrintResult(ReportProcessorItem::PrintNotFit);
		}
	}
	ReportItemMetaPaint mpit;
	//context().dump();
	while(singlePageProcessResult.value == ReportProcessorItem::PrintNotFit
		&& !(singlePageProcessResult.flags & ReportProcessorItem::FlagPrintNeverFit)) {
		singlePageProcessResult = processPage(&mpit);
		qfDebug() << "singlePageProcessResult:" << singlePageProcessResult.toString();
		//qfDebug().color(QFLog::Yellow) << context().styleCache().toString();
		//mpit.dump();
		ReportItemMetaPaint *it = mpit.firstChild();
		if(it) {
			it->setParent(f_processorOutput);
			if(mode == FirstPage || mode == SinglePage) {
				emit pageProcessed();
				if(singlePageProcessResult.value == ReportProcessorItem::PrintNotFit) {
					fProcessedPageNo++;
				}
				//qfInfo() << "pageProcessed:" << fProcessedPageNo;
				break;
			}
			else {
				if(singlePageProcessResult.value == ReportProcessorItem::PrintNotFit) {
					fProcessedPageNo++;
				}
				else {
					break;
				}
			}

		}
		else break;
	}
}

ReportProcessorItem::PrintResult ReportProcessor::processPage(ReportItemMetaPaint *out)
{
	qfLogFuncFrame();
	ReportProcessorItem::PrintResult res;
	if(processedItemsRoot()) {
		res = processedItemsRoot()->printMetaPaint(out, ReportProcessorItem::Rect());
		qfDebug() << "\tres:" << res.toString();
	}
	return res;
}

bool ReportProcessor::isProcessible(const QDomElement &el)
{
	static QSet<QString> set;
	if(set.isEmpty()) {
		set << "report" << "body" << "frame" << "row" << "cell"
				<< "para" << "table" << "band" << "detail"
				<< "image" << "graph"
				<< "space" << "break";
				//<< "script";
	}
	bool ret = set.contains(el.tagName());
	//if(!ret) { qfError() << "Element '" + el.tagName() + "' is not processible."; }
	return ret;
}

ReportProcessorItem* ReportProcessor::createProcessibleItem(const QDomElement &_el, ReportProcessorItem *parent)
{
	//qfLogFuncFrame() << "parent:" << ((parent)?parent->element.tagName(): "NULL") << "to create:" << _el.tagName();
	ReportProcessorItem *it = NULL;
	QDomElement el(_el);
	if(!isProcessible(el)) {
	//QF_ASSERT(isProcessible(_el), "Element '" + _el.tagName() + "' is not processible.");
		qfWarning() << tr("Element '%1' is not processible and it will be ignored.").arg(el.tagName());
	}
	else {
		{
			QString s = el.attribute("copyAttributesFromId");
			if(!s.isEmpty()) {
				QStringList sl = s.split(':');
				int level = 0;
				if(sl.count() > 1) {
					s = sl[0];
					level = sl[1].toInt();
				}
				report().copyAttributesFromId(el, s, level, true/*only_new*/);
			}
		}
		if(el.tagName() == "report") {
			if(el.attribute("headeronbreak").isEmpty()) el.setAttribute("headeronbreak", "1");
		}
		else if(el.tagName() == "row") {
			if(el.attribute("layout").isEmpty()) el.setAttribute("layout", "horizontal");
			if(el.attribute("keepall").isEmpty()) el.setAttribute("keepall", "1");
		}
		else if(el.tagName() == "cell") {
			//if(el.attribute("keepall").isEmpty()) el.setAttribute("keepall", "1");
		}
		else if(el.tagName() == "detail") {
			if(el.attribute("layout").isEmpty()) el.setAttribute("layout", "horizontal"); /// pro detail je defaultni layout horizontal.
			/// detail nemuze mit keepall, protoze pokud je detail vnorena tabulka a nevejde se na stranku, neni vytistena
			//if(el.attribute("keepall").isEmpty()) el.setAttribute("keepall", "1");
		}
		it = createItem(parent, el);
		//QF_ASSERT(it != NULL, el.tagName() + " element can not be created");
	}

	//qfDebug() << "\tcreated:" << it;
	//qfDebug() << "\tcreated child name:" << it->element.tagName();
	return it;
}

ReportProcessorItem * ReportProcessor::createItem(ReportProcessorItem * parent, const QDomElement & el) throw( QFException )
{
	ReportProcessorItem *it = NULL;
	if(el.tagName() == "report") {
		it = new ReportItemReport(this, el);
	}
	else if(el.tagName() == "body") {
		it = new ReportItemBody(this, parent, el);
	}
	else if(el.tagName() == "break") {
		it = new ReportItemBreak(this, parent, el);
	}
	else if(el.tagName() == "frame") {
		it = new ReportItemFrame(this, parent, el);
	}
	else if(el.tagName() == "image") {
		it = new ReportItemImage(this, parent, el);
	}
	else if(el.tagName() == "graph") {
		it = new ReportItemGraph(this, parent, el);
	}
	else if(el.tagName() == "space") {
		it = new ReportItemFrame(this, parent, el);
	}
	else if(el.tagName() == "row") {
		it = new ReportItemFrame(this, parent, el);
	}
	else if(el.tagName() == "cell") {
		it = new ReportItemFrame(this, parent, el);
	}
	else if(el.tagName() == "para") {
		it = new ReportItemPara(this, parent, el);
	}
	else if(el.tagName() == "band") {
		it = new ReportItemBand(this, parent, el);
	}
	else if(el.tagName() == "detail") {
		it = new ReportItemDetail(this, parent, el);
	}
	else if(el.tagName() == "table") {
		it = new ReportItemTable(this, parent, el);
	}
	QF_ASSERT(it != NULL, el.tagName() + " element can not be created");
	return it;
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

void ReportProcessor::processHtml(QDomElement & el_body) throw( QFException )
{
	processedItemsRoot()->resetIndexToPrintRecursively(ReportProcessorItem::IncludingParaTexts);
	processedItemsRoot()->printHtml(el_body);
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
	qfDebug() << "\t path:" << el.path();
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
	if(!m_qmlEngine) {
		m_qmlEngine = new QQmlEngine(this);
	}
	return m_qmlEngine;
#endif
}

void ReportProcessor::dump()
{
	if(f_processorOutput) f_processorOutput->dump();
}

void ReportProcessor::print(QPrinter &printer, const QVariantMap &options)
{
	qfLogFuncFrame();

	ReportPainter painter(&printer);

	typedef ReportProcessorItem::Rect Rect;
	//typedef ReportProcessorItem::Size Size;

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

QFDataTranslator* ReportProcessor::dataTranslator() const
{
	QFDataTranslator *ret = NULL;
	{
		QFAppDataTranslatorInterface *appi = dynamic_cast<QFAppDataTranslatorInterface *>(QCoreApplication::instance());
		if(appi) {
			ret = appi->dataTranslator();
		}
	}
	return ret;
}
