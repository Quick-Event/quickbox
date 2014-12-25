#include "headerview.h"

#include <qf/core/assert.h>

#include <QLabel>

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
