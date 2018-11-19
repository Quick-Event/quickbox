#include "headerview.h"

#include <qf/core/assert.h>

#include <QLabel>
#include <QContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QPainter>

using namespace qf::qmlwidgets;

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent) :
	QHeaderView(orientation, parent), m_seekStringLabel(nullptr)
{
	if(orientation == Qt::Horizontal) {
		m_seekStringLabel = new QLabel(this);
		QPalette palette;
		palette.setColor(QPalette::Active, QPalette::Window, QColor(255, 255, 127));
		m_seekStringLabel->setPalette(palette);
		m_seekStringLabel->setFrameShape(QFrame::StyledPanel);
		m_seekStringLabel->setAutoFillBackground(true);
		m_seekStringLabel->hide();

		setSectionsClickable(true);
		setSectionsMovable(true);
		//setSortIndicatorShown(false);
		setSortIndicator(-1, Qt::AscendingOrder);
		connect(this, &HeaderView::sortIndicatorChanged, [this]() {
			m_extraSortColumns.clear();
		});
	}
	else if(orientation == Qt::Vertical) {
		//verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	}
}

HeaderView::~HeaderView()
{
}

void HeaderView::setSeekString(const QString &str)
{
	QF_ASSERT(m_seekStringLabel != nullptr,
			  "Cannot set search string, m_searchStringLabel is NULL",
			  return);
	if(str.isEmpty()) {
		m_seekStringLabel->hide();
	}
	else {
		m_seekStringLabel->setText(str);
		m_seekStringLabel->move(sectionViewportPosition(sortIndicatorSection()), 0);
		m_seekStringLabel->resize(m_seekStringLabel->sizeHint());
		m_seekStringLabel->show();
		//qfInfo() << m_searchStringLabel->sizeHint().width() << "x" << m_searchStringLabel->sizeHint().height();
	}
}

void HeaderView::contextMenuEvent(QContextMenuEvent *event)
{
	if(orientation() != Qt::Horizontal)
		return;
	QAction a_sep(QString(), nullptr);
	a_sep.setSeparator(true);
	QAction a_resize_section(tr("Resize section to contents"), nullptr);
	//a_resize_section.setShortcut(tr("Ctrl-T", "Resize section to contents"));
	QAction a_resize_sections(tr("Resize all sections to contents"), nullptr);
	QAction a_hide_section(tr("Hide section"), nullptr);
	QAction a_show_all_sections(tr("Show all sections"), nullptr);
	QAction a_sections_natural_order(tr("Sections in natural order"), nullptr);
	QList<QAction*> lst;
	lst << &a_resize_section << &a_resize_sections
		<< &a_sep
		<< &a_hide_section << &a_show_all_sections
		<< &a_sep
		<< &a_sections_natural_order;
	QAction *a = QMenu::exec(lst, viewport()->mapToGlobal(event->pos()));
	if(a == &a_resize_section) {
		QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent());
		QF_ASSERT(view != nullptr, "Bad parent!", return);
		int log_ix = logicalIndexAt(event->pos());
		int sz = view->sizeHintForColumn(log_ix);
		sz = qMax(sz, sectionSizeHint(log_ix));
		sz = qMin(sz, maximumSectionSize());
		resizeSection(log_ix, sz);
	}
	else if(a == &a_resize_sections) {
		resizeSections(QHeaderView::ResizeToContents);
	}
	else if(a == &a_hide_section) {
		hideSection(logicalIndexAt(event->pos()));
	}
	else if(a == &a_show_all_sections) {
		for (int i = 0; i < count(); ++i) {
			if(isSectionHidden(i)) {
				showSection(i);
			}
		}
		reset(); // revealed columns are not sometimes visible without reset(), atleast in 5.5.1
	}
	else if(a == &a_sections_natural_order) {
		for (int i = 0; i < count(); ++i) {
			int log_i = logicalIndex(i);
			if(log_i == i)
				continue;
			for (int j = i+1; j < count(); ++j) {
				int log_j = logicalIndex(j);
				if(log_j == i) {
					moveSection(j, i);
					break;
				}
			}
		}
	}
}

void HeaderView::mousePressEvent(QMouseEvent *event)
{
	if(orientation() == Qt::Horizontal) {
		if(event->button() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier) {
			int ix = logicalIndexAt(event->x());
			//qfInfo() << "logical index:" << ix;
			m_extraSortColumns << ix;
			emit sortColumnAdded(ix);
			return;
		}
	}
	m_extraSortColumns.clear();
	Super::mousePressEvent(event);
}

void HeaderView::paintSection(QPainter *painter, const QRect &section_rect, int logical_index) const
{
	Super::paintSection(painter, section_rect, logical_index);
	if(orientation() != Qt::Horizontal)
		return;
	if (!m_extraSortColumns.contains(logical_index))
		return;
	painter->setClipping(false);
	auto indicator_rect = [](const QRect &section_rect) {
		QRect r;
		int h = section_rect.height();
		int w = section_rect.width();
		int x = section_rect.x();
		int y = section_rect.y();
		int arrow_w = h / 3;
		int horiz_margin = arrow_w / 2;
		int vert_margin = arrow_w;
		int arrow_h = arrow_w / 2;
		r.setRect(x + w - horiz_margin * 2 - arrow_w, y + vert_margin,
				  arrow_w, arrow_h);
		return r;
	};
	QRect rect = indicator_rect(section_rect);
	const QPen old_pen = painter->pen();
	const QBrush old_brush = painter->brush();
	painter->setPen(QPen(Qt::black, 0));
	painter->setBrush(Qt::yellow);
	if (sortIndicatorOrder() == Qt::AscendingOrder) {
		const QPoint points[] = {
			QPoint(rect.x() + rect.width() / 2, rect.y() + rect.height()),
			QPoint(rect.x(), rect.y()),
			QPoint(rect.x() + rect.width(), rect.y()),
		};
		painter->drawPolygon(points, sizeof points / sizeof *points);
	}
	else if (sortIndicatorOrder() == Qt::DescendingOrder) {
		const QPoint points[] = {
			QPoint(rect.x(), rect.y() + rect.height()),
			QPoint(rect.x() + rect.width(), rect.y() + rect.height()),
			QPoint(rect.x() + rect.width() / 2, rect.y()),
		};
		painter->drawPolygon(points, sizeof points / sizeof *points);
	}
	painter->setPen(old_pen);
	painter->setBrush(old_brush);
}
