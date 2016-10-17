#include "table.h"
#include "cellrenderer.h"

#include <qf/core/log.h>

#include <QTimer>

Table::Table(QWidget *parent)
	: Super(parent)
{
	qfLogFuncFrame();
	m_updateRowCountTimer = new QTimer(this);
	m_updateRowCountTimer->setSingleShot(true);
	connect(m_updateRowCountTimer, &QTimer::timeout, this, &Table::updateRowCount);
}

void Table::paintEvent(QPaintEvent *event)
{
	Super::paintEvent(event);
	if(m_rowCount == 0)
		return;
	ResultsCellRenderer cr(m_cellSize, this);
	int ix = 0;
	for (int j = 0; j < m_columnCount; ++j) {
		for (int i = 0; i < m_rowCount; ++i) {
			QPoint pos(j * m_cellSize.width(), i * m_cellSize.height());
			cr.draw(pos, this);
		}
	}
}

void Table::resizeEvent(QResizeEvent *event)
{
	Super::resizeEvent(event);
	m_updateRowCountTimer->start(100);
}

void Table::updateRowCount()
{
	qfLogFuncFrame() << m_cellSize;
	if(!m_cellSize.isValid()) {
		QFontMetrics fm(font(), this);
		qfInfo() << "h:" << fm.height() << "asc:" << fm.ascent() << "desc:" << fm.descent();
		m_cellSize.setHeight(fm.height() * 2);
	}
	QSize frame_size = geometry().size();
	m_rowCount = frame_size.height() / m_cellSize.height();
	int rest = frame_size.height() % m_cellSize.height();
	m_cellSize.setHeight(m_cellSize.height() + rest / m_rowCount);
	m_cellSize.setWidth(frame_size.width() / m_columnCount);
	qfDebug() << "new row count:" << m_rowCount << "cell size:" << m_cellSize;
	update();
}
