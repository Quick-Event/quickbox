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
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
	m_scaledLetterWidth = static_cast<int>(fm.horizontalAdvance('W') * m_fontScale);
#else
	m_scaledLetterWidth = fm.width('W') * m_fontScale;
#endif
	m_cellSpacing = m_scaledLetterWidth / 2;
}

//=========================================================
// ClassCellRenderer
//=========================================================
ClassCellRenderer::ClassCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
	m_cellAttributes.resize(ColumnCount);
	m_cellAttributes[Name] = CellAttribute{size.width() / 3 - m_cellSpacing};
	m_cellAttributes[Info] = CellAttribute{2 * size.width() / 3 - 2 * m_cellSpacing, Qt::AlignRight};
}

void ClassCellRenderer::draw(const QPoint &position, const QVariantMap &data, QWidget *widget)
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
	painter.fillRect(r.adjusted(1, 1, -1, -1), QColor("gold"));
	int x = 0;
	for (int i = 0; i < ColumnCount; ++i) {
		painter.save();
		x += m_cellSpacing;
		painter.translate(x, 0);
		QRect cell_rect = r;
		cell_rect.setLeft(0);
		cell_rect.setRight(m_cellAttributes[i].width);
		//painter.drawRect(cell_rect);
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
// RunnersCellRenderer
//=========================================================
RunnersListCellRenderer::RunnersListCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
}

void RunnersListCellRenderer::draw(const QPoint &position, const QVariantMap &data, QWidget *widget)
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

	painter.fillRect(r, QColor(Qt::gray));
	painter.fillRect(r.adjusted(1, 1, -1, -1), QColor("midnightblue"));

	int x = 0;
	for (int i = 0; i < columnCount(); ++i) {
		painter.save();
		x += m_cellSpacing;
		painter.translate(x, 0);
		QRect cell_rect = r.adjusted(1, 1, -1, -1);
		cell_rect.setLeft(0);
		cell_rect.setRight(m_cellAttributes[i].width);
		//painter.fillRect(cell_rect, QColor("midnightblue"));
		//painter.drawRect(cell_rect);
		painter.setPen(QPen(Qt::white));

		painter.scale(m_fontScale, m_fontScale);
		QRect text_rect(QPoint(), cell_rect.size() / m_fontScale);
		QString text = columnText(i, record);
		painter.drawText(text_rect, m_cellAttributes[i].alignment, text);
		x += m_cellAttributes[i].width;
		painter.restore();
	}
}

//=========================================================
// StartListCellRenderer
//=========================================================
StartListCellRenderer::StartListCellRenderer(const QSize &size, QWidget *widget)
	: Super(size, widget)
{
	m_cellAttributes.resize(ColumnCount);
	m_cellAttributes[StartTime] = CellAttribute{6 * m_scaledLetterWidth, Qt::AlignRight};
	m_cellAttributes[Name] = CellAttribute{0};
	m_cellAttributes[Registration] = CellAttribute{7 * m_scaledLetterWidth};
	m_cellAttributes[SiId] = CellAttribute{8 * m_scaledLetterWidth, Qt::AlignRight};
	int sum = m_cellSpacing;
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

QString StartListCellRenderer::columnText(int col, const QVariantMap &data)
{
	QString ret;
	switch(col) {
	case StartTime: {
		int sec = data.value(QStringLiteral("starttimems")).toInt() / 1000;
		ret = QString("%1.%2").arg(sec / 60).arg(sec % 60, 2, 10, QChar('0'));
		return ret;
	}
	case Name: ret = QStringLiteral("{{lastname}} {{firstname}}"); break;
	case Registration: ret = QStringLiteral("{{registration}}"); break;
	case SiId: ret = QStringLiteral("{{siid}}"); break;
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
	m_cellAttributes[Position] = CellAttribute{4 * m_scaledLetterWidth, Qt::AlignRight};
	m_cellAttributes[Name] = CellAttribute{0};
	m_cellAttributes[Registration] = CellAttribute{7 * m_scaledLetterWidth};
	m_cellAttributes[Time] = CellAttribute{7 * m_scaledLetterWidth, Qt::AlignRight};
	m_cellAttributes[Status] = CellAttribute{5 * m_scaledLetterWidth};
	int sum = m_cellSpacing;
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

QString ResultsCellRenderer::columnText(int col, const QVariantMap &data)
{
	QString ret;
	switch(col) {
	case Position: {
		bool disq = data.value(QStringLiteral("disqualified")).toBool();
		if(disq)
			return QString();
		ret = QStringLiteral("{{pos}}.");
		break;
	}
	case Name: ret = QStringLiteral("{{lastname}} {{firstname}}"); break;
	case Registration: ret = QStringLiteral("{{registration}}"); break;
	case Time: {
		int sec = data.value(QStringLiteral("timems")).toInt() / 1000;
		ret = QString("%1.%2").arg(sec / 60).arg(sec % 60, 2, 10, QChar('0'));
		return ret;
	}
	case Status: {
		bool disq = data.value(QStringLiteral("disqualified")).toBool();
		return disq? tr("DISQ"): QString();
	}
	default:
		ret = QStringLiteral("WTF");
	}
	//qfInfo() << ret << "---->" << qf::core::Utils::replaceCaptions(ret, data);
	ret = qf::core::Utils::replaceCaptions(ret, data);
	return ret;
}

