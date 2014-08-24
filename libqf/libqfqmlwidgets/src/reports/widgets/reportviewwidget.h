//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTVIEWWIDGET_H
#define QF_QMLWIDGETS_REPORTS_REPORTVIEWWIDGET_H

#include "../../qmlwidgetsglobal.h"
#include "../../framework/dialogwidget.h"

#include <qf/core/exception.h>
#include <qf/core/utils/treetable.h>

#include <QScrollArea>
#include <QFrame>
#include <QMatrix>

class QFAction;
class QFReportItemMetaPaint;
class QFReportItemMetaPaintReport;
class QFReportItemMetaPaintFrame;
class QFReportPainter;
class QFReportProcessor;
class QFStatusBar;
class QLineEdit;
class QSpinBox;
class QFDataTranslator;
class QPrinter;

namespace qf {
namespace qmlwidgets {
namespace reports {

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT  ReportViewWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
protected:
	class ScrollArea;
	class PainterWidget;
	PainterWidget *f_painterWidget;
	ScrollArea *f_scrollArea;

	QFAction::ActionList f_actionList;
	QFUiBuilder *f_uiBuilder;

	//QFReportItemMetaPaint *fDocument;
	QFTreeTable fData;
	int fCurrentPageNo;
	qreal f_scale;

	int whenRenderingSetCurrentPageTo;

	QLineEdit *edCurrentPage;

	QFReportItemMetaPaint *f_selectedItem;
	QMatrix painterInverseMatrix;

	QFReportProcessor *f_reportProcessor;

	static const int PageBorder = 5;

	QFStatusBar *f_statusBar;
private:
	void selectElement_helper(QFReportItemMetaPaint *it, const QFDomElement &el);
	void selectItem(const QPointF &p);
	bool selectItem_helper(QFReportItemMetaPaint *it, const QPointF &p);
protected:
	QFStatusBar *statusBar();
	QSpinBox *zoomStatusSpinBox;

	/// nastavi painteru scale a offset
	void setupPainter(QFReportPainter *p);
	//! nastavi velikost widgetu podle rozmeru aktualni stranky a aktualniho zvetseni.
	void setupPainterWidgetSize();

	void refreshWidget();
	void refreshActions();

	virtual void attachPrintout() {}

	void print() throw(QFException);
	void exportPdf(const QString &file_name) throw(QFException);
	QString exportHtml() throw(QFException);

	QFDataTranslator* dataTranslator() const;
signals:
	//! pokud je report vytisknut nepo exportovan do PDF
	//void reportPrinted();
	void elementSelected(const QFDomElement &el);
	void sqlValueEdited(const QString &sql_id, const QVariant &val);
protected slots:
	void edCurrentPageEdited();
	void pageProcessed();
	void scrollToPrevPage() {view_prevPage(ScrollToPageEnd);}
	void scrollToNextPage() {view_nextPage(ScrollToPageTop);}
	void setScaleProc(int proc) {setScale(proc * 0.01);}
public:
	enum PageScrollPosition {ScrollToPageTop, ScrollToPageEnd};
public slots:
	void print(QPrinter *printer) throw(QFException); /// slot ktery potrebuje print preview
	/// prerendruje report
	void render();
	//! Zacne prekladat report a jak pribyvaji stranky, zobrazuji se ve view, nemuzu pro to pouzit specialni thread,
	//! protoze QFont musi byt pouzivan v GUI threadu, tak prekladam stranku po strance pomoci QTimer::singleShot()
	void processReport();
	void selectElement(const QFDomElement &el);

	//void printOnDefaultPrinter() throw(QFException);

	virtual void file_print();
	virtual void file_printPreview();
	virtual void file_export_pdf(bool open = false);
	virtual void file_export_pdf_open() {file_export_pdf(true);}
	virtual void file_export_html();
	virtual void file_export_email();
	virtual void report_edit();

	void data_showHtml();

	void view_nextPage(PageScrollPosition scroll_pos = ScrollToPageTop);
	void view_prevPage(PageScrollPosition scroll_pos = ScrollToPageTop);
	void view_firstPage();
	void view_lastPage();
	void view_zoomIn(const QPoint &center_pos = QPoint());
	void view_zoomOut(const QPoint &center_pos = QPoint());
	void view_zoomToFitWidth();
	void view_zoomToFitHeight();
	void zoomOnWheel(int delta, const QPoint &pos);
public:
	virtual QFReportProcessor* reportProcessor();
	//! does not take ownership of \a proc
	//! connect necessarry signals and slots
	void setReportProcessor(QFReportProcessor *proc);

	virtual void setVisible(bool visible);

	///=================== INTERFACE ================
	virtual QFPart::ToolBarList createToolBars();
	//virtual bool hasMenubar() {return true;}
	virtual void updateMenuOrBar(QWidget *menu_or_menubar) {f_uiBuilder->updateMenuOrBar(menu_or_menubar);}
	///===============================================

	QFReportItemMetaPaintReport* document(bool throw_exc = Qf::ThrowExc) throw(QFException);
	// widget does not take the ownership of the document \a doc .
	//void setDocument(QFReportItemMetaPaint* doc);
	const QFDomDocument& data() const;
	void setData(const QFDomDocument &_data);
	void setData(const QFSValue &_data) {fData = _data;}
	//! Volani teto funkce zpusobi prelozeni reportu, vlozeni pripadnych dat a jeho zobrazeni.
	void setReport(const QString &file_name);
	void setReport(const QFDomDocument &doc);

	/// stranky se pocitaji od 0
	int currentPageNo() const {return fCurrentPageNo;}
	/// stranky se pocitaji od 0
	void setCurrentPageNo(int pg_no);
	int pageCount();
	QFReportItemMetaPaintFrame *currentPage();
	/// return NULL if such a page does not exist.
	QFReportItemMetaPaintFrame *getPage(int page_no);

	qreal scale() const {return f_scale;}
	void setScale(qreal _scale);

	QFAction* action(const QString &name, bool throw_exc = Qf::ThrowExc) throw(QFException)
	{
		return QFUiBuilder::findAction(f_actionList, name, throw_exc);
	}

	QFReportItemMetaPaint* selectedItem() const {return f_selectedItem;}
	//const QList<QFToolBar*>& toolBars() {return fToolBarList;}
	virtual void prePrint() {}
	void print(QPrinter &printer, const QVariantMap &options = QVariantMap()) throw(QFException);
public:
	QFReportViewWidget(QWidget *parent = NULL);
	virtual ~QFReportViewWidget();
};

class QFReportViewWidget::PainterWidget : public QWidget
{
	Q_OBJECT
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void paintEvent(QPaintEvent *event);
	QFReportViewWidget* reportViewWidget();
	/// screen dots per mm
signals:
	//void zoomOnWheel(int delta, const QPoint &pos);
	void sqlValueEdited(const QString &sql_id, const QVariant &val);
public:
	PainterWidget(QWidget *parent);
	virtual ~PainterWidget() {}
};

class QFReportViewWidget::ScrollArea : public QScrollArea
{
	Q_OBJECT
protected:
	QPoint f_dragMouseStartPos;
	QPoint f_dragViewportStartPos;
protected:
	virtual void keyPressEvent(QKeyEvent *ev);
	virtual void keyReleaseEvent(QKeyEvent *ev);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev);
	//virtual void dragMoveEvent(QDragMoveEvent *ev);
signals:
	void showNextPage();
	void showPreviousPage();
protected slots:
	void verticalScrollBarValueChanged(int value);
signals:
	void zoomOnWheel(int delta, const QPoint &pos);
public:
	ScrollArea(QWidget *parent);
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTVIEWWIDGET_H

