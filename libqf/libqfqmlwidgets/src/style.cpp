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

}

QPixmap Style::pixmapFromSvg(const QString &name, const QSize &pixmap_size)
{
	QPixmap ret;
	QString file_name = iconPath() + '/' + name;
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
			ret = QPixmap(pixmap_size);
			ret.fill(Qt::transparent);
			QPainter painter(&ret);
			QRect r(QPoint(0, 0), pixmap_size);
			QSize svg_sz = rnd.defaultSize();
			if(svg_sz.height() < svg_sz.width()) {
				int new_h = pixmap_size.width() * svg_sz.height() / svg_sz.width();
				int pos = (pixmap_size.width() - new_h) / 2;
				r = QRect(0, pos, pixmap_size.width(), new_h - pos);
			}
			else if(svg_sz.width() < svg_sz.height()) {
				int new_w = pixmap_size.height() * svg_sz.width() / svg_sz.height();
				int pos = (pixmap_size.width() - new_w) / 2;
				r = QRect(pos, 0, new_w - pos, pixmap_size.height());
			};
			rnd.render(&painter, r);
		}
	}
	return ret;
}

QIcon Style::icon(const QString &name)
{
	QIcon ret;
	QPixmap pxm = pixmapFromSvg(name, defaultIconSize());
	if(!pxm.isNull()) {
		ret.addPixmap(pxm);
		return ret;
	}
	ret.addFile(iconPath() + '/' + name);
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

