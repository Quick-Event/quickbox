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

class QLineEdit;
class QSpinBox;
class QPrinter;

namespace qf {
namespace qmlwidgets {

class Action;
class StatusBar;

namespace reports {

class ReportItemMetaPaint;
class ReportItemMetaPaintReport;
class ReportItemMetaPaintFrame;
class ReportPainter;
class ReportProcessor;


class QFQMLWIDGETS_DECL_EXPORT  ReportViewWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	ReportViewWidget(QWidget *parent = NULL);
	~ReportViewWidget() Q_DECL_OVERRIDE;

	/**
	 * @brief showReport
	 * @param parent
	 * @param report_qml_file
	 * @param single_table_data
	 * @param window_title
	 * @param config_persistent_id
	 * @param report_init_properties
	 * @return true if report was printed
	 */
	static bool showReport(QWidget *parent
			, const QString &report_qml_file
			, const QVariant &single_table_data
			, const QString &window_title = tr("Report preview")
			, const QString &config_persistent_id = QString()
			, const QVariantMap &report_init_properties = QVariantMap());
	static bool showReport2(QWidget *parent
			, const QString &report_qml_file
			, const QVariantMap &multiple_table_data
			, const QString &window_title = tr("Report preview")
			, const QString &persistent_settings_id = QString()
			, const QVariantMap &report_init_properties = QVariantMap());

protected:
	class ScrollArea;
	class PainterWidget;

	PainterWidget *m_painterWidget;
	ScrollArea *m_scrollArea;

	int m_currentPageNo;
	qreal m_scale;

	int m_whenRenderingSetCurrentPageTo;

	QLineEdit *edCurrentPage;

	ReportItemMetaPaint *m_selectedItem;
	QMatrix m_painterInverseMatrix;

	ReportProcessor *m_reportProcessor;

	static const int PageBorder = 5;

	qf::qmlwidgets::StatusBar *m_statusBar;
private:
	void selectItem(const QPointF &p);
	ReportItemMetaPaint* selectItem_helper(ReportItemMetaPaint *it, const QPointF &p);
protected:
	qf::qmlwidgets::StatusBar* statusBar();
	QSpinBox *zoomStatusSpinBox;

	/// nastavi painteru scale a offset
	void setupPainter(ReportPainter *p);
	//! nastavi velikost widgetu podle rozmeru aktualni stranky a aktualniho zvetseni.
	void setupPainterWidgetSize();

	void refreshWidget();
	void refreshActions();

	void print();
	void exportPdf(const QString &file_name);
	QString exportHtml();
signals:
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
	void print(QPrinter *printer); /// slot ktery potrebuje print preview
	/// prerendruje report
	//void render();
	//! Zacne prekladat report a jak pribyvaji stranky, zobrazuji se ve view, nemuzu pro to pouzit specialni thread,
	//! protoze QFont musi byt pouzivan v GUI threadu, tak prekladam stranku po strance pomoci QTimer::singleShot()
	void processReport();

	virtual void file_print();
	virtual void file_printPreview();
	virtual void file_export_pdf(bool open = true);
	virtual void file_export_html();

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
	virtual ReportProcessor* reportProcessor();
	//! does not take ownership of \a proc
	//! connect necessarry signals and slots
	void setReportProcessor(ReportProcessor *proc);

	void setVisible(bool visible) Q_DECL_OVERRIDE;

	ReportItemMetaPaintReport* document(bool throw_exc = true);
	void setTableData(const QString &key, const qf::core::utils::TreeTable &table_data);
	Q_INVOKABLE void setTableData(const QString &key, const QVariant &table_data);
	Q_INVOKABLE void setTableData(const QVariant &data) {
		setTableData(QString(), data);
	}

	Q_SLOT void setReport(const QString &file_name, const QVariantMap &report_init_properties = QVariantMap());

	/// stranky se pocitaji od 0
	int currentPageNo() const {return m_currentPageNo;}
	/// stranky se pocitaji od 0
	void setCurrentPageNo(int pg_no);
	int pageCount();
	ReportItemMetaPaintFrame *currentPage();
	/// return NULL if such a page does not exist.
	ReportItemMetaPaintFrame *getPage(int page_no);

	qreal scale() const {return m_scale;}
	void setScale(qreal _scale);

	void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg) Q_DECL_OVERRIDE;
	ActionMap createActions() Q_DECL_OVERRIDE;

	ReportItemMetaPaint* selectedItem() const {return m_selectedItem;}
	virtual void prePrint() {}
	void print(QPrinter &printer, const QVariantMap &options = QVariantMap());
	Q_SIGNAL void reportPrinted(int printer_output_format);
private:
	QLineEdit *m_edCurrentPage = nullptr;
};

class ReportViewWidget::PainterWidget : public QWidget
{
	Q_OBJECT
protected:
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	//virtual void wheelEvent(QWheelEvent *event);
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	ReportViewWidget* reportViewWidget();
	/// screen dots per mm
signals:
	void sqlValueEdited(const QString &sql_id, const QVariant &val);
public:
	PainterWidget(QWidget *parent);
	virtual ~PainterWidget() {}
};

class ReportViewWidget::ScrollArea : public QScrollArea
{
	Q_OBJECT
protected:
	QPoint f_dragMouseStartPos;
	QPoint f_dragViewportStartPos;
protected:
	void keyPressEvent(QKeyEvent *ev) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent *ev) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
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

