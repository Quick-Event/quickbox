#include "cellrenderer.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QFontMetrics>
#include <QPainter>
#include <QWidget>

//=========================================================
// CellRenderer
//=========================================================
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

//=========================================================
// ClassCellRenderer
//=========================================================
ClassCellRenderer::ClassCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
	m_cellAttributes.resize(ColumnCount);
	m_cellAttributes[Name] = CellAttribute{size.width() / 2 - m_cellSpacing};
	m_cellAttributes[Info] = CellAttribute{size.width() / 2 - 2 * m_cellSpacing, Qt::AlignRight};
}

void ClassCellRenderer::draw(const QPoint &position, const QVariantMap &data, QWidget *widget)
{
	QVariantMap record = data.value(QStringLiteral("record")).toMap();
	//qfInfo() << record;
	QPainter painter(widget);
	//painter.fillRect(r.adjusted(2, 2, -2, -2), Qt::yellow);
	QPen pen(Qt::SolidLine);
	pen.setColor(Qt::red);
	painter.setPen(pen);
	painter.translate(position);
	QRect r(QPoint(0, 0), m_size);
	//qfDebug() << r;
	painter.fillRect(r.adjusted(1, 1, -1, -1), QColor("khaki"));
	int x = 0;
	for (int i = 0; i < ColumnCount; ++i) {
		painter.save();
		x += m_cellSpacing;
		painter.translate(x, 0);
		QRect cell_rect = r;
		cell_rect.setLeft(0);
		cell_rect.setRight(m_cellAttributes[i].width);
		painter.drawRect(cell_rect);
		painter.setPen(QPen(Qt::black));

		painter.scale(m_fontScale, m_fontScale);
		QRect text_rect(QPoint(), cell_rect.size() / m_fontScale);
		QString text = columnText((Column)i, record);
		painter.drawText(text_rect, m_cellAttributes[i].alignment, text);
		x += m_cellAttributes[i].width;
		painter.restore();
	}
}

QString ClassCellRenderer::columnText(ClassCellRenderer::Column col, const QVariantMap &data)
{
	QString ret;
	switch(col) {
	case Name: ret = QStringLiteral("{{name}}"); break;
	case Info: ret = tr("length: {{length}} climb: {{climb}}"); break;
	default:
		ret = QStringLiteral("WTF");
	}
	//qfInfo() << ret << "---->" << qf::core::Utils::replaceCaptions(ret, data);
	ret = qf::core::Utils::replaceCaptions(ret, data);
	return ret;
}

//=========================================================
// ResultsCellRenderer
//=========================================================
ResultsCellRenderer::ResultsCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
	m_cellAttributes.resize(ColumnCount);
	m_cellAttributes[Position] = CellAttribute{3 * m_scaledLetterWidth, Qt::AlignRight};
	m_cellAttributes[Name] = CellAttribute{0};
	m_cellAttributes[Registration] = CellAttribute{7 * m_scaledLetterWidth};
	m_cellAttributes[Time] = CellAttribute{6 * m_scaledLetterWidth, Qt::AlignRight};
	m_cellAttributes[Status] = CellAttribute{4 * m_scaledLetterWidth};
	int sum = 0;
	for (int i = 0; i < ColumnCount; ++i) {
		sum += m_cellSpacing;
		sum += m_cellAttributes[i].width;
	}
	int name_width = m_size.width() - sum;
	if(name_width < 0) {
		qfWarning() << "cell to narrow:" << m_size.width() << "should be at least:" << sum;
		name_width = 0;
	}
	m_cellAttributes[Name].width = name_width;
}

void ResultsCellRenderer::draw(const QPoint &position, const QVariantMap &data, QWidget *widget)
{
	QVariantMap record = data.value(QStringLiteral("record")).toMap();
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
		cell_rect.setRight(m_cellAttributes[i].width);
		//painter.fillRect(cell_rect, QColor("green"));
		painter.drawRect(cell_rect);
		painter.setPen(QPen(Qt::white));

		painter.scale(m_fontScale, m_fontScale);
		QRect text_rect(QPoint(), cell_rect.size() / m_fontScale);
		QString text = columnText((Column)i, record);
		painter.drawText(text_rect, m_cellAttributes[i].alignment, text);
		x += m_cellAttributes[i].width;
		painter.restore();
	}
}

QString ResultsCellRenderer::columnText(ResultsCellRenderer::Column col, const QVariantMap &data)
{
	QString ret;
	switch(col) {
	case Position: ret = QStringLiteral("{{pos}}."); break;
	case Name: ret = QStringLiteral("{{lastname}} {{firstname}}"); break;
	case Registration: ret = QStringLiteral("{{registration}}"); break;
	case Time: {
		int sec = data.value(QStringLiteral("timems")).toInt() / 1000;
		ret = QString("%1.%2").arg(sec / 60).arg(sec % 60, 2, 10, QChar('0'));
		return ret;
	}
	case Status: ret = QStringLiteral("{{status}}"); break;
	default:
		ret = QStringLiteral("WTF");
	}
	//qfInfo() << ret << "---->" << qf::core::Utils::replaceCaptions(ret, data);
	ret = qf::core::Utils::replaceCaptions(ret, data);
	return ret;
}


