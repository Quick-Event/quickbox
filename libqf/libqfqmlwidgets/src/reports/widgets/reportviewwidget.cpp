//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#include "reportviewwidget.h"
#include "itemvalueeditorwidget.h"

#include "../processor/reportpainter.h"
#include "../processor/reportprocessor.h"
#include "../../action.h"
#include "../../dialogs/dialog.h"
#include "../../dialogs/filedialog.h"
#include "../../statusbar.h"
#include "../../framework/cursoroverrider.h"

#include <qf/core/utils/fileutils.h>
#include <qf/core/log.h>
#include <qf/core/string.h>

#include <QScrollBar>
#include <QMenu>
#include <QBoxLayout>
#include <QSpinBox>
#include <QBitmap>
#include <QTimer>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QLineEdit>
#include <QDomDocument>
#include <QDir>

#include <typeinfo>

namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets::reports;

//====================================================
//         ReportViewWidget::ScrollArea
//====================================================
ReportViewWidget::ScrollArea::ScrollArea(QWidget * parent)
: QScrollArea(parent)
{
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScrollBarValueChanged(int)));
}

void ReportViewWidget::ScrollArea::wheelEvent(QWheelEvent * ev)
{
	if(ev->orientation() == Qt::Vertical) {
		if(ev->modifiers() == Qt::ShiftModifier) {
			int delta = ev->delta();
			if (ev->orientation() == Qt::Horizontal) {
			}
			else {
				emit zoomOnWheel(delta, ev->pos());
			}
			ev->accept();
			return;
		}
		else {
			QScrollBar *sb = verticalScrollBar();
			if(!sb || !sb->isVisible()) {
				/// pokud neni scroll bar, nemuzu se spolehnout na funkci verticalScrollBarValueChanged(), protoze value je pro oba smery == 0
				//qfInfo() << e->delta();
				if(ev->delta() < 0) {
					emit showNextPage();
				}
				else {
					emit showPreviousPage();
				}
				ev->accept();
				return;
			}
		}
	}
	QScrollArea::wheelEvent(ev);
}

void ReportViewWidget::ScrollArea::keyPressEvent(QKeyEvent* ev)
{
	if(ev->modifiers() == Qt::ShiftModifier) {
		static QCursor c;
		if(c.bitmap() == NULL) {
			QBitmap b1(":/libqfgui/images/zoomin_cursor_bitmap.png");
			QBitmap b2(":/libqfgui/images/zoomin_cursor_mask.png");
			c = QCursor(b1, b2, 18, 12);
		}
		setCursor(c);
	}
	QAbstractScrollArea::keyPressEvent(ev);
}

void ReportViewWidget::ScrollArea::keyReleaseEvent(QKeyEvent* ev)
{
	setCursor(QCursor());
	QWidget::keyReleaseEvent(ev);
}

void ReportViewWidget::ScrollArea::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::ShiftModifier) {
		f_dragMouseStartPos = ev->pos();
		f_dragViewportStartPos = -widget()->pos();
		setCursor(QCursor(Qt::ClosedHandCursor));
		//QPoint pos = ev->pos();
		//qfInfo() << "press start pos:" << f_dragStartPos.x() << f_dragStartPos.y() << "pos:" << pos.x() << pos.y();
		//QDrag *drag = new QDrag(this);
		//QMimeData *mimeData = new QMimeData;
		//mimeData->setText(commentEdit->toPlainText());
		//drag->setMimeData(mimeData);
		//drag->setPixmap(iconPixmap);

		//Qt::DropAction dropAction = drag->exec();
		ev->accept();
		return;
	}
	QScrollArea::mousePressEvent(ev);
}

void ReportViewWidget::ScrollArea::mouseReleaseEvent(QMouseEvent* ev)
{
	setCursor(QCursor());
	QAbstractScrollArea::mouseReleaseEvent(ev);
}

void ReportViewWidget::ScrollArea::mouseMoveEvent(QMouseEvent* ev)
{
	//qfInfo() << "move buttons:" << ev->buttons() << ev->button() << "mod:" << QString::number(ev->modifiers(), 16) << (ev->button() == Qt::LeftButton) << (ev->modifiers() == Qt::ShiftModifier);
	if(ev->buttons() == Qt::LeftButton && ev->modifiers() == Qt::ShiftModifier) {
		QPoint pos = ev->pos();
		//qfInfo() << "start pos:" << f_dragStartPos.x() << f_dragStartPos.y() << "pos:" << pos.x() << pos.y();
		/// dej startpos na pos
		QScrollBar *hsb = horizontalScrollBar();
		QScrollBar *vsb = verticalScrollBar();
		//QSize ws = widget()->size();
		//QRect wr = widget()->geometry();
		if(hsb) {
			hsb->setValue(f_dragViewportStartPos.x() - (pos.x() - f_dragMouseStartPos.x()));
			//qfDebug() << "\t horizontal scrollbar value2:" << hsb->value() << "of" << hsb->maximum();
		}
		if(vsb) {
			vsb->setValue(f_dragViewportStartPos.y() - (pos.y() - f_dragMouseStartPos.y()));
			//qfDebug() << "\t vertical scrollbar value2:" << vsb->value() << "of" << vsb->maximum();
		}
		ev->accept();
		return;
	}
	QScrollArea::mouseMoveEvent(ev);
}

void ReportViewWidget::ScrollArea::verticalScrollBarValueChanged(int value)
{
	//qfLogFuncFrame() << "value:" << value;
	//qfInfo() << value;
	static int old_val = -1;
	QScrollBar *sb = verticalScrollBar();
	if(value == old_val) {
		if(value == sb->maximum()) {
			emit showNextPage();
		}
		else if(value == sb->minimum()) {
			emit showPreviousPage();
		}
	}
	old_val = value;
}

//====================================================
//        ReportViewWidget::PainterWidget
//====================================================
ReportViewWidget::PainterWidget::PainterWidget(QWidget *parent)
	: QWidget(parent)
{
	//screenDPMm = QPainter(this).device()->physicalDpiX() / 25.4;
	//setAutoFillBackground(true);
	//resize(210*2+20, 297*2+20);
}

ReportViewWidget* ReportViewWidget::PainterWidget::reportViewWidget()
{
	return qf::core::findParent<ReportViewWidget*>(this);
}

void ReportViewWidget::PainterWidget::paintEvent(QPaintEvent *ev)
{
	//qfDebug() << QF_FUNC_NAME;
	QWidget::paintEvent(ev);
	ReportPainter painter(this);
	painter.setMarkEditableSqlText(true);

	/// nakresli ramecek a stranku
	//painter.setBrush(Qt::yellow);
	QRect r1 = rect();
	painter.fillRect(r1, QBrush(QColor("#CCFF99")));
	//int d = (int)(ReportViewWidget::PageBorder * reportViewWidget()->scale());
	//r.adjust(d, d, -d, -d);
	/*
	QPen p(Qt::blue);
	p.setBrush(QColor("red"));
	//p.setWidth(2);
	painter.setPen(p);
	painter.drawRect(rect().adjusted(0, 0, -1, -1));
	painter.setBrush(Qt::green);
	QFont f("autobus", 30);
	f.setStyleHint(QFont::Times);
	painter.setFont(f);
	//painter.drawText(r, Qt::AlignCenter | Qt::TextWordWrap, "<qt>Qt <b>kjutyn</b> <br>indian</qt>");
	*/
	reportViewWidget()->setupPainter(&painter);
	ReportItemMetaPaintFrame *frm = reportViewWidget()->currentPage();
	if(!frm) return;
	graphics::Rect r = graphics::mm2device(frm->renderedRect, painter.device());
	painter.fillRect(r, QColor("white"));
	painter.setPen(QColor("teal"));
	painter.setBrush(QBrush());
	painter.drawRect(r);

	painter.drawMetaPaint(reportViewWidget()->currentPage());
	//painter.setPen(p);
	//QRect r(0, 0, 210, 297);
	//painter.drawText(r, Qt::AlignCenter | Qt::TextWordWrap, "<qt>Qt <b>kjutyn</b> <br>indian</qt>");
}

void ReportViewWidget::PainterWidget::mousePressEvent(QMouseEvent *e)
{
	qfLogFuncFrame();
	if(e->button() == Qt::LeftButton && e->modifiers() == Qt::ShiftModifier) {
		QWidget::mousePressEvent(e); /// ignoruj posouvani reportu mysi
	}
	else {
		ReportItemMetaPaint::Point p = QPointF(e->pos());
		p = reportViewWidget()->painterInverseMatrix.map(p);
		p = graphics::device2mm(p, this);
		qfDebug() << Q_FUNC_INFO << ReportItem::Point(p).toString();
		reportViewWidget()->selectItem(p);
		ReportItemMetaPaint *selected_item = reportViewWidget()->selectedItem();
		if(selected_item) {
			if(e->button() == Qt::RightButton) {
				//qfInfo() << "\t item type:" << typeid(*selected_item).name();
				ReportItemMetaPaintText *it = dynamic_cast<ReportItemMetaPaintText*>(selected_item->firstChild());
				if(it) {
					QMenu menu(this);
					menu.setTitle(tr("Item menu"));
					QAction *act_edit = menu.addAction(tr("Editovat text"));
					/*--
					{
						QStringList edit_grants = qf::core::String(it->editGrants).splitAndTrim(',');
						bool edit_enabled = edit_grants.isEmpty();
						QFApplication *app = qobject_cast<QFApplication*>(QCoreApplication::instance());
						qfDebug() << "app:" << app << "edit grants:" << edit_grants.join(",") << "edits enabled:" << edit_enabled;
						if(app) foreach(QString grant, edit_grants)  {
							if(app->currentUserHasGrant(grant)) {
								edit_enabled = true;
								break;
							}
						}
						act_edit->setEnabled(edit_enabled);
					}
					--*/
					act_edit->setEnabled(true);
					QAction *a = menu.exec(mapToGlobal(e->pos()));
					if(a == act_edit) {
						ItemValueEditorWidget *w = new ItemValueEditorWidget();
						w->setValue(it->text);
						dialogs::Dialog dlg(this);
						dlg.setCentralWidget(w);
						if(dlg.exec()) {
							QVariant val = w->value();
							it->text = val.toString();
							/// nevim proc, ale \n mi to tiskne, jako zvonecek :)
							it->text.replace('\n', QChar::LineSeparator);
							/// roztahni text na ohranicujici ramecek, aby se tam delsi text vesel
							it->setRenderedRectRect(selected_item->renderedRect);
							update();
							QString sql_id = it->sqlId;
							if(!sql_id.isEmpty()) {
								emit sqlValueEdited(sql_id, val);
							}
						}
					}
				}
			}
		}
	}
}

void ReportViewWidget::PainterWidget::wheelEvent(QWheelEvent *event)
{
	/*
	if(event->modifiers() == Qt::ShiftModifier) {
		int delta = event->delta();
		//int numSteps = numDegrees / 15;

		if (event->orientation() == Qt::Horizontal) {
			//scrollHorizontally(numSteps);
		} else {
			emit zoomOnWheel(delta, event->pos());
		}
		event->accept();
	}
	else
		*/
		QWidget::wheelEvent(event);
}

//====================================================
//                                 ReportViewWidget
//====================================================
ReportViewWidget::ReportViewWidget(QWidget *parent)
	: Super(parent), f_scrollArea(NULL), edCurrentPage(NULL), f_statusBar(NULL)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	f_reportProcessor = NULL;
	whenRenderingSetCurrentPageTo = -1;
	/*--
	f_uiBuilder = new QFUiBuilder(this, ":/libqfgui/qfreportviewwidget.ui.xml");
	f_actionList += f_uiBuilder->actions();
	--*/
	//--action("file.export.email")->setVisible(false);
	//action("report.edit")->setVisible(false);

	fCurrentPageNo = -1;
	f_selectedItem = NULL;

	f_scrollArea = new ScrollArea(NULL);
	/// zajimavy, odkomentuju tenhle radek a nemuzu nastavit pozadi zadnyho widgetu na scrollArea.
	//f_scrollArea->setBackgroundRole(QPalette::Dark);
	f_painterWidget = new PainterWidget(f_scrollArea);
	connect(f_painterWidget, SIGNAL(sqlValueEdited(QString,QVariant)), this, SIGNAL(sqlValueEdited(QString,QVariant)), Qt::QueuedConnection);
	connect(f_scrollArea, SIGNAL(zoomOnWheel(int,QPoint)), this, SLOT(zoomOnWheel(int,QPoint)), Qt::QueuedConnection);
	f_scrollArea->setWidget(f_painterWidget);
	QBoxLayout *ly = new QVBoxLayout(this);
	ly->setSpacing(0);
	ly->setMargin(0);
	ly->addWidget(f_scrollArea);
	ly->addWidget(statusBar());

	connect(f_scrollArea, SIGNAL(showNextPage()), this, SLOT(scrollToNextPage()));
	connect(f_scrollArea, SIGNAL(showPreviousPage()), this, SLOT(scrollToPrevPage()));

	//--QFUiBuilder::connectActions(f_actionList, this);
}

ReportViewWidget::~ReportViewWidget()
{
	qfLogFuncFrame() << this;
}

ReportProcessor * ReportViewWidget::reportProcessor()
{
	if(f_reportProcessor == NULL) {
		setReportProcessor(new ReportProcessor(f_painterWidget, this));
	}
	return f_reportProcessor;
}

void ReportViewWidget::setReportProcessor(ReportProcessor * proc)
{
	f_reportProcessor = proc;
	connect(f_reportProcessor, SIGNAL(pageProcessed()), this, SLOT(pageProcessed()));
}

qf::qmlwidgets::StatusBar *ReportViewWidget::statusBar()
{
	if(!f_statusBar) {
		f_statusBar = new qmlwidgets::StatusBar(NULL);
		zoomStatusSpinBox = new QSpinBox();
		zoomStatusSpinBox->setSingleStep(10);
		zoomStatusSpinBox->setMinimum(10);
		zoomStatusSpinBox->setMaximum(1000000);
		zoomStatusSpinBox->setPrefix("zoom: ");
		zoomStatusSpinBox->setSuffix("%");
		zoomStatusSpinBox->setAlignment(Qt::AlignRight);
		f_statusBar->addWidget(zoomStatusSpinBox);
		connect(zoomStatusSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setScaleProc(int)));
	}
	return f_statusBar;
}
/*--
QFPart::ToolBarList ReportViewWidget::createToolBars()
{
	qfDebug() << QF_FUNC_NAME;
	QFPart::ToolBarList tool_bars;
	//if(!f_toolBars.isEmpty()) return f_toolBars;

	tool_bars = f_uiBuilder->createToolBars();

	/// je to dost divoky, najdu toolbar jmenem "main" a v nem widget majici akci s id == "view.nextPage"
	/// a pred ni prdnu lineEdit, pukud pridam do toolbaru widget, musim na konec pridat i Stretch :(
	foreach(QFToolBar *tb, tool_bars) {
		//qfDebug() << "\ttoolbar object name:" << tb->objectName();
		if(tb->objectName() == "main") {
			QLayout *ly = tb->layout();
			//QF_ASSERT(ly, "bad layout cast");
			for(int i = 0; i < ly->count(); ++i) {
				QWidget *w = ly->itemAt(i)->widget();
				//qfDebug() << "\twidget:" << w;
				if(w) {
					QList<QAction*> alst = w->actions();
					if(!alst.isEmpty()) {
						QFAction *a = qobject_cast<QFAction*>(alst[0]);
						if(a) {
							//qfDebug() << "\taction id:" << a->id();
							if(a->id() == "view.nextPage") {
								edCurrentPage = new QLineEdit(NULL);
								edCurrentPage->setAlignment(Qt::AlignRight);
								edCurrentPage->setMaximumWidth(60);
								connect(edCurrentPage, SIGNAL(editingFinished()), this, SLOT(edCurrentPageEdited()));
								tb->insertWidget(a, edCurrentPage);
								QLabel *space = new QLabel(QString(), NULL);
								space->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
								tb->addWidget(space);
								//QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
								//ly->addItem(spacer);
								break;
							}
						}
					}
				}
			}
			break;
		}
	}

	return tool_bars;
}
--*/
void ReportViewWidget::view_zoomIn(const QPoint &center_pos)
{
	qfLogFuncFrame() << "center_pos:" << center_pos.x() << center_pos.y();
	const QRect visible_rect(-f_scrollArea->widget()->pos(), f_scrollArea->viewport()->size());
	//QSizeF old_report_size = f_scrollArea->widget()->size();
	QPointF old_abs_center_pos = visible_rect.topLeft() + center_pos;
	qfDebug() << "visible rect:" << qmlwidgets::graphics::Rect(visible_rect).toString();
	QScrollBar *hsb = f_scrollArea->horizontalScrollBar();
	QScrollBar *vsb = f_scrollArea->verticalScrollBar();

	qreal old_scale = scale(), new_scale;
	//QPointF old_report_pos = QPointF(abs_center_pos) / old_scale;
	//if(old_scale > 1) new_scale = old_scale + 1;
	//else if(old_scale > 1) new_scale = old_scale + 1;
	//else
	new_scale = old_scale * 1.33;
	setScale(new_scale);
	//qfInfo() << "new scale:" << new_scale << "old scale:" << old_scale;
	if(!center_pos.isNull()) {
		QPointF new_abs_center_pos = old_abs_center_pos * (new_scale / old_scale);
		if(hsb) {
			hsb->setValue(new_abs_center_pos.x() - center_pos.x());
			qfDebug() << "\t horizontal scrollbar value2:" << hsb->value() << "of" << hsb->maximum();
		}
		if(vsb) {
			vsb->setValue(new_abs_center_pos.y() - center_pos.y());
			qfDebug() << "\t vertical scrollbar value2:" << vsb->value() << "of" << vsb->maximum();
		}
	}
}

void ReportViewWidget::view_zoomOut(const QPoint &center_pos)
{
	qfLogFuncFrame() << "center_pos:" << center_pos.x() << center_pos.y();
	const QRect visible_rect(-f_scrollArea->widget()->pos(), f_scrollArea->viewport()->size());
	//QSizeF old_report_size = f_scrollArea->widget()->size();
	QPointF old_abs_center_pos = visible_rect.topLeft() + center_pos;
	qfDebug() << "visible rect:" << qmlwidgets::graphics::Rect(visible_rect).toString();
	QScrollBar *hsb = f_scrollArea->horizontalScrollBar();
	QScrollBar *vsb = f_scrollArea->verticalScrollBar();

	qreal old_scale = scale(), new_scale;
	//QPointF old_report_pos = QPointF(abs_center_pos) / old_scale;
	//if(old_scale > 2) new_scale = old_scale - 1;
	//else
	new_scale = old_scale / 1.33;
	setScale(new_scale);

	if(!center_pos.isNull()) {
		QPointF new_abs_center_pos = old_abs_center_pos * (new_scale / old_scale);
		if(hsb) {
			hsb->setValue(new_abs_center_pos.x() - center_pos.x());
			qfDebug() << "\t horizontal scrollbar value2:" << hsb->value() << "of" << hsb->maximum();
		}
		if(vsb) {
			vsb->setValue(new_abs_center_pos.y() - center_pos.y());
			qfDebug() << "\t vertical scrollbar value2:" << vsb->value() << "of" << vsb->maximum();
		}
	}
}

void ReportViewWidget::zoomOnWheel(int delta, const QPoint &center_pos)
{
	if(delta < 0) view_zoomIn(center_pos);
	else if(delta > 0) view_zoomOut(center_pos);
}

void ReportViewWidget::view_zoomToFitWidth()
{
	qfDebug() << QF_FUNC_NAME;
	ReportItemMetaPaintFrame *frm = currentPage();
	if(!frm) return;
	ReportItemMetaPaintFrame::Rect r = frm->renderedRect;
	double report_px = (r.width() + 2*PageBorder) * logicalDpiX() / 25.4;
	double widget_px = f_scrollArea->width();
	//QScrollBar *sb = f_scrollArea->verticalScrollBar();
	//if(sb) widget_px -= sb->width();
	double sc = widget_px / report_px * 0.98;
	setScale(sc);
}

void ReportViewWidget::view_zoomToFitHeight()
{
	ReportItemMetaPaintFrame *frm = currentPage();
	if(!frm) return;
	ReportItemMetaPaintFrame::Rect r = frm->renderedRect;
	double report_px = (r.height() + 2*PageBorder) * f_painterWidget->logicalDpiY() / 25.4;
	double widget_px = f_scrollArea->height();
	double sc = widget_px / report_px * 0.98;
	setScale(sc);
}

void ReportViewWidget::setScale(qreal _scale)
{
	qfLogFuncFrame() << currentPageNo();
	ReportItemMetaPaintFrame *frm = currentPage();
	if(!frm)
		return;

	f_scale = _scale;
	setupPainterWidgetSize();
	f_painterWidget->update();
	refreshWidget();
}

void ReportViewWidget::setupPainterWidgetSize()
{
	qfLogFuncFrame();
	ReportItemMetaPaintFrame *frm = currentPage();
	if(!frm)
		return;
	qmlwidgets::graphics::Rect r1 = frm->renderedRect.adjusted(-PageBorder, -PageBorder, PageBorder, PageBorder);
	qmlwidgets::graphics::Rect r2 = qmlwidgets::graphics::mm2device(r1, f_painterWidget);
	//qfDebug() << "\tframe rect:" << r.toString();
	QSizeF s = r2.size();
	s *= scale();
	//painterScale = QSizeF(s.width() / r1.width(), s.height() / r1.height());
	f_painterWidget->resize(s.toSize());
}

void ReportViewWidget::setupPainter(ReportPainter *p)
{
	QF_ASSERT(p != nullptr, "painter is NULL", return);
	//qfInfo() << QF_FUNC_NAME;
	//qfInfo() << "\t painterScale:" << ReportItem::Size(painterScale).toString();
	//p->currentPage = currentPageNo();
	p->pageCount = pageCount();
	//QFDomElement el;
	//if(f_selectedItem) el = f_selectedItem->reportElement;
	p->setSelectedItem(f_selectedItem);
	//if(f_selectedItem) qfInfo() << "painter selected item:" << f_selectedItem->reportItem()->path().toString() << f_selectedItem->reportItem()->element.tagName();
	p->scale(scale(), scale());
	//p->scale(painterScale.width(), painterScale.height());
	//qfInfo() << "\t painter world matrix m11:" << p->worldMatrix().m11() << "m12:" << p->worldMatrix().m12();
	//qfInfo() << "\t painter world matrix m21:" << p->worldMatrix().m21() << "m22:" << p->worldMatrix().m22();
	p->translate(qmlwidgets::graphics::mm2device(qmlwidgets::graphics::Point(PageBorder, PageBorder), p->device()));
	painterInverseMatrix = p->matrix().inverted();
}
/*
void ReportViewWidget::setDocument(ReportItemMetaPaint* doc)
{
	qfDebug() << QF_FUNC_NAME;
	fDocument = doc;
	if(!doc) return;
	//doc->dump();
}
	*/
void ReportViewWidget::setReport(const QString &file_name)
{
	qfLogFuncFrame() << "file_name:" << file_name;
	//qfDebug() << "\tdata:" << fData.toString();
	reportProcessor()->setReport(file_name);
	//out.dump();
}

void ReportViewWidget::pageProcessed()
{
	qfDebug() << QF_FUNC_NAME;
	if(whenRenderingSetCurrentPageTo >= 0) {
		if(pageCount() - 1 == whenRenderingSetCurrentPageTo) {
			setCurrentPageNo(whenRenderingSetCurrentPageTo);
			whenRenderingSetCurrentPageTo = -1;
		}
	}
	else {
		if(pageCount() == 1) setCurrentPageNo(0);
	}
	//QApplication::processEvents();
	refreshWidget();
	//setCurrentPageNo(0);
	QTimer::singleShot(10, reportProcessor(), SLOT(processSinglePage())); /// 10 je kompromis mezi rychlosti prekladu a sviznosti GUI
}

ReportItemMetaPaintReport* ReportViewWidget::document(bool throw_exc)
{
	ReportItemMetaPaintReport *doc = reportProcessor()->processorOutput();
	if(!doc && throw_exc)
		QF_EXCEPTION("document is NULL");
	return doc;
}

void ReportViewWidget::setData(const qf::core::utils::TreeTable &data)
{
	qfLogFuncFrame();
	reportProcessor()->setData(data);
}

void ReportViewWidget::setData(const QVariant &data)
{
	qfu::TreeTable tt;
	tt.setVariant(data);
	setData(tt);
}

int ReportViewWidget::pageCount()
{
	//qfLogFuncFrame();
	int ret = 0;
	if(document(!qf::core::Exception::Throw)) {
		ret = document()->childrenCount();
	}
	else {
		//qfDebug() << "\tdocument is null";
	}
	//qfDebug() << "\treturn:" << ret;
	return ret;
}

void ReportViewWidget::setCurrentPageNo(int pg_no)
{
	if(pg_no >= pageCount() || pg_no < 0)
		pg_no = 0;
	fCurrentPageNo = pg_no;
	setupPainterWidgetSize();
	f_painterWidget->update();
	refreshWidget();
}

ReportItemMetaPaintFrame* ReportViewWidget::getPage(int n)
{
	//qfDebug() << QF_FUNC_NAME << currentPageNo();
	if(!document(!qf::core::Exception::Throw))
		return NULL;
	if(n < 0 || n >= document()->childrenCount())
		return NULL;
	ReportItemMetaPaint *it = document()->child(n);
	ReportItemMetaPaintFrame *frm = dynamic_cast<ReportItemMetaPaintFrame*>(it);
	//qfDebug() << "\treturn:" << frm;
	return frm;
}

ReportItemMetaPaintFrame* ReportViewWidget::currentPage()
{
	ReportItemMetaPaintFrame *frm = getPage(currentPageNo());
	//qfDebug() << QF_FUNC_NAME << currentPageNo();
	if(!frm)
		return NULL;
	return frm;
}
/*--
void ReportViewWidget::selectElement_helper(ReportItemMetaPaint *it, const QFDomElement &el)
{
	QDomElement el1 = it->reportItem()->element;
	if(el1 == el) {
		//qfInfo() << "BINGO";
		f_selectedItem = it;
		qfDebug() << "\t EMIT:" << el1.tagName();
		emit elementSelected(el1);
	}
	else {
		foreach(QFTreeItemBase *_it, it->children()) {
			ReportItemMetaPaint *it1 = static_cast<ReportItemMetaPaint*>(_it);
			selectElement_helper(it1, el);
		}
	}
}

void ReportViewWidget::selectElement(const QFDomElement &el)
{
	qfLogFuncFrame() << el.tagName();
	if(!el) return;
	//qfInfo() << __LINE__;
	#if 0
	kdyz se tohle odkomentuje, tak to pada
	if(f_selectedItem && f_selectedItem->reportItem()->element == el) {
		qfDebug() << "\t allready selected";
		return;
	}
	#endif
	f_selectedItem = NULL;
	ReportItemMetaPaintFrame *frm = currentPage();
	if(!frm) return;
	selectElement_helper(frm, el);
	f_painterWidget->update();
}

static QString is_fake_element(const QFDomElement &_el)
{
	qfLogFuncFrame();
	QString fake_path;
	QFDomElement el = _el;
	while(!!el) {
		//qfError() << el.toString();
		fake_path = el.attribute("__fakePath");
		qfDebug() << "\t fake path of" << el.tagName() << "=" << fake_path;
		if(!fake_path.isEmpty()) break;
		else if(el.hasAttribute("__fake")) {
			fake_path = "-";
			break;
		}
		el = el.parentNode().toElement();
	}
	qfDebug() << "\t return:" << fake_path;
	return fake_path;
}
--*/
bool ReportViewWidget::selectItem_helper(ReportItemMetaPaint *it, const QPointF &p)
{
	bool ret = false;
	if(it->isPointInside(p)) {
		ret = true;
		qfLogFuncFrame() << "point inside:" << it->reportItem() << it->renderedRect.toString();
		//qfInfo() << it->dump();
		bool in_child = false;
		foreach(qfu::TreeItemBase *_it, it->children()) {
			ReportItemMetaPaint *it1 = static_cast<ReportItemMetaPaint*>(_it);
			if(selectItem_helper(it1, p)) {
				in_child = true;
				break;
			}
		}
		if(!in_child) {
			//bool selectable_element_found = false;
			if(f_selectedItem != it) {
				f_selectedItem = it;
				//qfInfo() << "selected item:" << f_selectedItem->reportItem()->path().toString() << f_selectedItem->reportItem()->element.tagName();
				/// muze se stat, ze item nema element, pak hledej u rodicu
				while(it) {
					ReportItem *r_it = it->reportItem();
					if(r_it) {
						f_painterWidget->update();
#ifdef QML_SELECT_ITEM_IMPLEMENTED
						/// pokus se najit nejaky rozumny element pro tento report item
						QFDomElement el = r_it->element;
						if(!!el) {
							if(el.attribute("unselectable").toBool()) { ret = false; break; }
							/// nalezeny element nesmi pochazet z fakeBand, pozna se to tak, ze element nebo nektery z jeho predku ma atribut "__fake"
							QString fake_path = is_fake_element(el);
							//qfInfo() << "fake path:" << fake_path;
							if(!fake_path.isEmpty()) {
								/// pokud je to fake element, bylo by fajn, kdyby se emitoval element, ze ktereho byl fake element vytvoren
								/// najdi element table
								ReportItemTable *t_it = NULL;
								while(r_it) {
									t_it = dynamic_cast<ReportItemTable*>(r_it);
									if(t_it) break;
									r_it = r_it->parent();
								}
								QDomElement faked_el;
								if(t_it) {
									if(fake_path == "-") {
										faked_el = t_it->element;
									}
									else {
										//qfInfo() << "cd" << fake_path;
										el = t_it->element;
										faked_el = el.cd(fake_path, !qf::core::Exception::Throw);
										/*
										if(!faked_el.isNull()) {
											qfInfo() << "\tOK";
											//selectable_element_found = true;
											//qfDebug() << "element:" << fSelectedElement.toString();
										}
										*/
									}
									qfDebug() << "\t EMIT:" << faked_el.tagName();
									emit elementSelected(faked_el);
								}
							}
							else {
								//selectable_element_found = true;
								//qfDebug() << "element:" << fSelectedElement.toString();
								qfDebug() << "\t EMIT:" << el.tagName();
								emit elementSelected(el);
							}
						}
#endif
					}
					else {
						//qfWarning() << "item for path:" << it->path().toString() << "NOT FOUND.";
					}
					break;
					//it = it->parent();
				}
			}
			//return true;
		}
		//return true;
	}
	return ret;
}

void ReportViewWidget::selectItem(const QPointF &p)
{
	qfLogFuncFrame();
	ReportItemMetaPaintFrame *frm = currentPage();
	ReportItemMetaPaint *old_selected_item = f_selectedItem;
	//QFDomElement old_el = fSelectedElement;
	f_selectedItem = NULL;
	if(frm) selectItem_helper(frm, p);
	if(!f_selectedItem && old_selected_item) {
		/// odznac puvodni selekci
		f_painterWidget->update();
	}
}

void ReportViewWidget::setVisible(bool visible)
{
	qfLogFuncFrame() << "visible:" << visible;
	//setCurrentPageNo(0);
	Super::setVisible(visible);
	//setCurrentPageNo(0);
	//QTimer::singleShot(0, this, SLOT(processReport()));
	if(visible)
		processReport();
}

void ReportViewWidget::processReport()
{
	qfLogFuncFrame();
	if(!reportProcessor()->processorOutput()) {
		reportProcessor()->process(ReportProcessor::FirstPage);
	}
	setCurrentPageNo(0);
	setScale(1);
}

void ReportViewWidget::render()
{
	qfLogFuncFrame();
	whenRenderingSetCurrentPageTo = currentPageNo();
	reportProcessor()->reset();
	if(!reportProcessor()->processorOutput()) {
		//qfInfo() << "process report";
		reportProcessor()->process(ReportProcessor::FirstPage);
	}
	//qfInfo() << "setCurrentPageNo:" << cur_page_no;
	//setCurrentPageNo(cur_page_no);
}

void ReportViewWidget::refreshWidget()
{
	statusBar();
	if(edCurrentPage)
		edCurrentPage->setText(QString::number(currentPageNo()+1) + "/" + QString::number(pageCount()));
	refreshActions();
	zoomStatusSpinBox->setValue((int)(scale() * 100));
	//statusBar()->setText("zoom: " + QString::number((int)(scale() * 100)) + "%");
}

void ReportViewWidget::refreshActions()
{
	/*--
	int pgno = currentPageNo();
	int pgcnt = pageCount();
	action("view.firstPage")->setEnabled(pgno > 0 && pgcnt > 0);
	action("view.prevPage")->setEnabled(pgno > 0 && pgcnt > 0);
	action("view.nextPage")->setEnabled(pgno < pgcnt - 1);
	action("view.lastPage")->setEnabled(pgno < pgcnt - 1);
	--*/
}

void ReportViewWidget::view_nextPage(PageScrollPosition scroll_pos)
{
	qfDebug() << QF_FUNC_NAME;
	if(currentPageNo() < pageCount() - 1) {
		setCurrentPageNo(currentPageNo() + 1);
		if(scroll_pos == ScrollToPageTop) f_scrollArea->verticalScrollBar()->setValue(f_scrollArea->verticalScrollBar()->minimum());
		else if(scroll_pos == ScrollToPageEnd) f_scrollArea->verticalScrollBar()->setValue(f_scrollArea->verticalScrollBar()->maximum());
	}
}

void ReportViewWidget::view_prevPage(PageScrollPosition scroll_pos)
{
	qfDebug() << QF_FUNC_NAME;
	if(currentPageNo() > 0) {
		setCurrentPageNo(currentPageNo() - 1);
		if(scroll_pos == ScrollToPageTop) f_scrollArea->verticalScrollBar()->setValue(f_scrollArea->verticalScrollBar()->minimum());
		else if(scroll_pos == ScrollToPageEnd) f_scrollArea->verticalScrollBar()->setValue(f_scrollArea->verticalScrollBar()->maximum());
	}
}

void ReportViewWidget::view_firstPage()
{
	qfDebug() << QF_FUNC_NAME;
	int pgno = 0;
	if(pgno < pageCount()) setCurrentPageNo(pgno);
}

void ReportViewWidget::view_lastPage()
{
	qfDebug() << QF_FUNC_NAME;
	int pgno = pageCount() - 1;
	if(pgno >= 0) setCurrentPageNo(pgno);
}

void ReportViewWidget::edCurrentPageEdited()
{
	qfDebug() << QF_FUNC_NAME;
	QStringList sl = edCurrentPage->text().split("/");
	if(sl.count() > 0) {
		int pg = sl[0].toInt() - 1;
		setCurrentPageNo(pg);
	}
}

void ReportViewWidget::print(QPrinter *printer)
{
	if(printer)
		print(*printer);
}

void ReportViewWidget::print(QPrinter &printer, const QVariantMap &options)
{
	qfLogFuncFrame();

	framework::CursorOverrider cov(Qt::WaitCursor);
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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

	//QApplication::restoreOverrideCursor();
	//emit reportPrinted();
}

void ReportViewWidget::print()
{
	qfLogFuncFrame();

	QPrinter printer;
	printer.setOutputFormat(QPrinter::NativeFormat);
	//printer.setOutputFileName(fn);
	printer.setFullPage(true);
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(document()->orientation);

	QPrintDialog dlg(&printer, this);
	if(dlg.exec() != QDialog::Accepted) return;

	qfDebug() << "options:" << dlg.options();
	QVariantMap opts;
	if(dlg.testOption(QAbstractPrintDialog::PrintPageRange)) { /// tohle je nastaveny vzdycky :(
		int from_page = dlg.fromPage();
		int to_page = dlg.toPage();
		qfDebug() << "fromPage:" << dlg.fromPage() << "toPage:" << dlg.toPage();
		if(from_page > 0) opts["fromPage"] = dlg.fromPage();
		if(to_page > 0) opts["toPage"] = dlg.toPage();
	}

	print(printer, opts);
}

void ReportViewWidget::exportPdf(const QString &file_name)
{
	qfLogFuncFrame();
	QString fn = file_name;
	if(fn.isEmpty())
		QF_EXCEPTION(tr("empty file name"));
	if(!fn.toLower().endsWith(".pdf"))
		fn += ".pdf";

	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fn);
	printer.setFullPage(true);
	printer.setPageSize(QPrinter::A4);
	printer.setOrientation(document()->orientation);
	/// zatim bez dialogu
	//QPrintDialog printDialog(&printer, parentWidget());
	//if(printDialog.exec() != QDialog::Accepted) return;

	print(printer);
}

namespace {
QString addHtmlEnvelope(const QString &html_body_content)
{
	QString eoln = QStringLiteral("\n");
	QString ret;
	ret += "<html>" + eoln;
	ret += "<head>" + eoln;
	ret += "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>" + eoln;
	ret += "<title>Data</title>" + eoln;
	ret += "<style type=\"text/css\">"
			"th {background-color:khaki; font-weight:bold;}"
			"table {border-collapse: collapse;}"
			"</style>" + eoln;
	ret += "</head>" + eoln;
	ret += "<body>" + eoln;
	ret += html_body_content;
	ret += "</body>" + eoln;
	ret += "</html>" + eoln;
	return ret;
}

}
QString ReportViewWidget::exportHtml()
{
	qfLogFuncFrame();
	QDomDocument doc;
	doc.setContent(addHtmlEnvelope(QString()));
	QDomElement el_body = doc.documentElement().firstChildElement("body");
	framework::CursorOverrider cov(Qt::WaitCursor);
	//QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	reportProcessor()->processHtml(el_body);
	//QApplication::restoreOverrideCursor();
	return doc.toString(2);
}

void ReportViewWidget::file_export_pdf(bool open)
{
	qfDebug() << QF_FUNC_NAME;
	attachPrintout();
	//reportProcessor()->dump();
	QString fn;
	if(open)
		fn = QDir::tempPath() + "/report.pdf";
	else
		fn = dialogs::FileDialog::getSaveFileName (this, tr("Save as PDF"), QString(), "*.pdf");
	if(fn.isEmpty())
		return;
	if(!fn.endsWith(".pdf", Qt::CaseInsensitive))
		fn += ".pdf";
	exportPdf(fn);
	if(open) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
	}
}

void ReportViewWidget::file_export_html()
{
	qfLogFuncFrame();
	QString fn = "report.html";
	QString s = exportHtml();
	qfError() << Q_FUNC_INFO << "NIY";
	/*--
	QFHtmlViewUtils hut(!QFHtmlViewUtils::UseWebKit);
	hut.showOrSaveHtml(s, fn, "showOrSaveHtml");
	--*/
}

void ReportViewWidget::file_export_email()
{
	/// uloz pdf do tmp file report.pdf
	attachPrintout();
	QString fn = QDir::tempPath() + "/report.pdf";
	exportPdf(fn);
}

void ReportViewWidget::data_showHtml()
{
	qfDebug() << QF_FUNC_NAME;
	QString s = reportProcessor()->data().toHtml();
	s = addHtmlEnvelope(s);
	QString file_name = "data.html";
	qfError() << Q_FUNC_INFO << "showing" << file_name << "not implemented yet.";
	/*--
	QFHtmlViewUtils hu(!QFHtmlViewUtils::UseWebKit);
	hu.showOrSaveHtml(s, file_name);
	--*/
}

void ReportViewWidget::file_print()
{
	qfDebug() << QF_FUNC_NAME;
	attachPrintout();
	print();
}

void ReportViewWidget::file_printPreview()
{
	qfLogFuncFrame();
	QPrinter printer(QPrinter::ScreenResolution);	/// SVG to tiskne na preview moc velky, pokud je QPrinter printer(QPrinter::HighResolution), je to potreba opravit
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));
	preview.exec();
}

void ReportViewWidget::report_edit()
{
	qfLogFuncFrame();
	// will not be possible in GUI with qmlreports
#if 0
	QString program = qfu::FileUtils::appDir() + "/repedit";
	#if defined Q_OS_WIN
	program += ".exe";
	#endif
	QStringList arguments;
	{
		QFSqlSearchDirs *sql_sd = dynamic_cast<QFSqlSearchDirs*>(reportProcessor()->searchDirs(!qf::core::Exception::Throw));
		if(sql_sd) {
			QFAppDbConnectionInterface *appi = dynamic_cast<QFAppDbConnectionInterface*>(QCoreApplication::instance());
			if(appi) {
				QFSqlConnection &c = appi->connection();
				/// user:password@host:port
				QString sql_connection = c.userName() % ':' % c.password() % '@' % c.hostName() % ':' % QString::number(c.port());
				/*
				QString sql_connection = "sql://" % c.userName() % ':' % c.password() % '@' % c.hostName() % ':' % QString::number(c.port())
				% '/' % c.databaseName()
				% '/' % sql_sd->tableName()
				% '/' % sql_sd->ckeyColumnName()
				% '/' % sql_sd->dataColumnName();
				*/
				arguments << "--sql-connection=" + sql_connection;
				//arguments << "--dbfs-profile=" + sql_connection;
				arguments << "--report-search-sql-dirs=" + sql_sd->sqlDirs().join("::");
			}
		}
	}
	{
		QFSearchDirs *sd = dynamic_cast<QFSearchDirs*>(reportProcessor()->searchDirs(!qf::core::Exception::Throw));
		if(sd) {
			arguments << "--report-search-dirs=" + sd->dirs().join("::");
			if(sd->dirs().count()) arguments << "--saved-files-root-dir=" + sd->dirs().first();
		}
	}
	QFDataTranslator *dtr = dataTranslator();
	if(dtr) {
		QString lc_domain = dtr->currentLocalesName();
		if(!lc_domain.isEmpty()) arguments << "--force-lc-domain=" + lc_domain;
	}
	arguments << reportProcessor()->report().fileName();
	//arguments << "-dqfdom";
#if defined QT_DEBUG /// aby se mi netisklo heslo do logu v release verzi
	qfDebug() << "\t command line:" << program << arguments.join(" ");
#endif
	QProcess *proc = new QProcess(qfApp());
	proc->start(program, arguments);
#endif
}





