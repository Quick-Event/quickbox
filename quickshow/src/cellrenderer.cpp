#include "cellrenderer.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QFontMetrics>
#include <QPainter>
#include <QWidget>

CellRenderer::CellRenderer(const QSize &size, QWidget *widget)
	: m_size(size)
{
	QFont font = widget->font();
	QFontMetrics fm(font, widget);
	m_fontAscent = fm.ascent();
	m_fontDescent = fm.descent();
	m_fontScale = m_size.height() / (double)(m_fontAscent + m_fontDescent);
	m_scaledLetterWidth = fm.width('X') * m_fontScale;
	m_cellSpacing = m_scaledLetterWidth / 2;
}

ResultsCellRenderer::ResultsCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
	m_cellWidths.resize(ColumnCount);
	m_cellWidths[Position] = 3 * m_scaledLetterWidth;
	m_cellWidths[Name] = 0;
	m_cellWidths[Registration] = 7 * m_scaledLetterWidth;
	m_cellWidths[Time] = 6 * m_scaledLetterWidth;
	m_cellWidths[Status] = 4 * m_scaledLetterWidth;
	int sum = 0;
	for (int i = 0; i < ColumnCount; ++i) {
		sum += m_cellSpacing;
		sum += m_cellWidths[i];
	}
	int name_width = m_size.width() - sum;
	if(name_width < 0) {
		qfWarning() << "cell to narrow:" << m_size.width() << "should be at least:" << sum;
		name_width = 0;
	}
	m_cellWidths[Name] = name_width;
}

void ResultsCellRenderer::draw(const QPoint &position, const QVariantMap &data, QWidget *widget)
{
	qfInfo() << data;
	QPainter painter(widget);
	//painter.fillRect(r.adjusted(2, 2, -2, -2), Qt::yellow);
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::red);
	painter.setPen(pen);
	painter.translate(position);
	QRect r(QPoint(0, 0), m_size);
	//qfDebug() << r;
	painter.fillRect(r.adjusted(1, 1, -1, -1), QColor("midnightblue"));
	//painter.drawRect(r);
	//painter.setBrush(Qt::white);
	//double scale = m_size.height() / (double)(m_fontAscent + m_fontDescent);
	//painter.scale(scale, scale);
	int x = 0;
	for (int i = 0; i < ColumnCount; ++i) {
		painter.save();
		x += m_cellSpacing;
		painter.translate(x, 0);
		QRect cell_rect = r;//.adjusted(0, 2, 0, -2);
		cell_rect.setLeft(0);
		cell_rect.setRight(m_cellWidths[i]);
		//painter.fillRect(cell_rect, QColor("green"));
		painter.drawRect(cell_rect);
		painter.setPen(QPen(Qt::white));
		painter.scale(m_fontScale, m_fontScale);
		QPoint pos = QPoint(0, m_fontAscent);
		QString text = columnText((Column)i, data);
		//qfDebug() << "\t" << cell_rect << text;
		painter.drawText(pos, text);
		x += m_cellWidths[i];
		painter.restore();
	}
}

QString ResultsCellRenderer::columnText(ResultsCellRenderer::Column col, const QVariantMap &data)
{
	QString ret;
	switch(col) {
	case Position: ret = QStringLiteral("{{position}}."); break;
	case Name: ret = QStringLiteral("{{fullName}}"); break;
	case Registration: ret = QStringLiteral("{{registration}}"); break;
	case Time: ret = QStringLiteral("{{type}}"); break;
	case Status: ret = QStringLiteral("{{status}}"); break;
	default:
		ret = QStringLiteral("WTF");
	}
	//qfInfo() << ret << "---->" << qf::core::Utils::replaceCaptions(ret, data);
	ret = qf::core::Utils::replaceCaptions(ret, data);
	return ret;
}

