#include "headerview.h"

#include <qf/core/assert.h>

#include <QLabel>

using namespace qf::qmlwidgets;

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent) :
	QHeaderView(orientation, parent), m_searchStringLabel(nullptr)
{
	if(orientation == Qt::Horizontal) {
		m_searchStringLabel = new QLabel(this);
		QPalette palette;
		palette.setColor(QPalette::Active, QPalette::Window, QColor(255, 255, 127));
		m_searchStringLabel->setPalette(palette);
		m_searchStringLabel->setFrameShape(QFrame::StyledPanel);
		m_searchStringLabel->setAutoFillBackground(true);
		m_searchStringLabel->hide();

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

void HeaderView::setSearchString(const QString &str)
{
	QF_ASSERT(m_searchStringLabel != nullptr,
			  "Cannot set search string, m_searchStringLabel is NULL",
			  return);
	if(str.isEmpty()) {
		m_searchStringLabel->hide();
	}
	else {
		m_searchStringLabel->setText(str);
		m_searchStringLabel->move(sectionViewportPosition(sortIndicatorSection()), 0);
		m_searchStringLabel->resize(m_searchStringLabel->sizeHint());
		m_searchStringLabel->show();
		//qfInfo() << m_searchStringLabel->sizeHint().width() << "x" << m_searchStringLabel->sizeHint().height();
	}
}
