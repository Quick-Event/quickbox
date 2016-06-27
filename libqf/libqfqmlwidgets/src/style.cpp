#include "style.h"

#include <qf/core/log.h>

#include <QCoreApplication>
#include <QIcon>
#include <QSvgRenderer>
#include <QFile>
#include <QPainter>

using namespace qf::qmlwidgets;

static const char *PROPERTY_STYLE_INSTANCE = "qf::qmlwidget::Style::instance";

Style::Style(QObject *parent)
	: QObject(parent)
{
	m_defaultIconSize = QSize(16, 16);
	//setDefaultIconSize(QSize(16, 16));
}

QPixmap Style::pixmapFromSvg(const QString &name, const QSize &pixmap_size)
{
	QPixmap ret;
	QSize px_sz = pixmap_size;
	if(px_sz.isEmpty())
		px_sz = defaultIconSize();
	QString file_name = name;
	if(!file_name.startsWith(QLatin1String(":/")))
		file_name = iconPath() + '/' + name;
	{
		QString svg_file_name = file_name;
		if(!name.endsWith(QLatin1String(".svg"))) {
			// try is SVG version exists
			svg_file_name = svg_file_name + ".svg";
		}
		QFile f(svg_file_name);
		if(f.open(QFile::ReadOnly)) {
			QByteArray ba = f.readAll();
			QSvgRenderer rnd(ba);
			ret = QPixmap(px_sz);
			ret.fill(Qt::transparent);
			QPainter painter(&ret);
			QRect r(QPoint(0, 0), px_sz);
			QSize svg_sz = rnd.defaultSize();
			if(svg_sz.height() < svg_sz.width()) {
				int new_h = px_sz.width() * svg_sz.height() / svg_sz.width();
				int pos = (px_sz.width() - new_h) / 2;
				r = QRect(0, pos, px_sz.width(), new_h - pos);
			}
			else if(svg_sz.width() < svg_sz.height()) {
				int new_w = px_sz.height() * svg_sz.width() / svg_sz.height();
				int pos = (px_sz.width() - new_w) / 2;
				r = QRect(pos, 0, new_w - pos, px_sz.height());
			};
			rnd.render(&painter, r);
		}
	}
	return ret;
}

QPixmap Style::pixmap(const QString &name, const QSize &pixmap_size)
{
	QSize sz = pixmap_size;
	if(sz.isEmpty())
		sz = defaultIconSize();
	QPixmap ret = pixmapFromSvg(name, sz);
	if(ret.isNull()) {
		QString file_name = name;
		if(!file_name.startsWith(QLatin1String(":/")))
			file_name = iconPath() + '/' + name;
		ret = QPixmap(file_name);
		if(!ret.isNull())
			ret = ret.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	return ret;
}

QPixmap Style::pixmap(const QString &name, int height)
{
	return pixmap(name, QSize(height, height));
}

QIcon Style::icon(const QString &name, const QSize &pixmap_size)
{
	QIcon ret;
	ret.addPixmap(pixmap(name, pixmap_size));
	return ret;
}

Style *Style::instance()
{
	QCoreApplication *app = QCoreApplication::instance();
	if(app) {
		QVariant v = app->property(PROPERTY_STYLE_INSTANCE);
		QObject *o = v.value<QObject*>();
		auto *style = qobject_cast<Style*>(o);
		if(!style) {
			style = new Style(app);
			QVariant v = QVariant::fromValue(style);
			app->setProperty(PROPERTY_STYLE_INSTANCE, v);
		}
		return style;
	}
	qfError() << "qf::qmlwidget::Style::instance is available only when QCoreApplication instance exists.";
	return nullptr;
}

void Style::setInstance(Style *style)
{
	QCoreApplication *app = QCoreApplication::instance();
	if(app) {
		QVariant v = QVariant::fromValue(style);
		app->setProperty(PROPERTY_STYLE_INSTANCE, v);
	}
	qfError() << "qf::qmlwidget::Style::instance is available only when QCoreApplication instance exists.";
}

