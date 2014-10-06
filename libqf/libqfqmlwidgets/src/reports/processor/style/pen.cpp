#include "pen.h"
#include "color.h"
#include "sheet.h"
#include "../reportprocessor.h"

#include <qf/core/log.h>

#include <QColor>

using namespace qf::qmlwidgets::reports::style;

Pen::Pen(QObject *parent) :
	QObject(parent), IStyled(this)
{
	setName(nextSequentialName());
	m_dirty = true;
}

Pen::~Pen()
{

}

QPen Pen::pen()
{
	if(m_dirty) {
		m_dirty = false;
		{
			QVariant v = basedOn();
			if(v.isValid()) {
				QObject *o = v.value<QObject*>();
				Pen *based_on = qobject_cast<Pen*>(o);
				if(!based_on) {
					if(v.type() == QVariant::String) {
						based_on = penForName(v.toString());
					}
				}
				if(!based_on) {
					qfWarning() << "Invalid pen basedOn definition" << v.toString() << "in:" << this;
				}
				else {
					m_pen = based_on->pen();
				}
			}
		}
		{
			QVariant v = color();
			if(v.isValid()) {
				QColor c;
				if(v.type() == QVariant::String) {
					c.setNamedColor(v.toString());
				}
				else {
					QObject *o = v.value<QObject*>();
					Color *pco = qobject_cast<Color*>(o);
					if(pco) {
						c = pco->color();
					}
				}
				if(c.isValid()) {
					m_pen.setColor(c);
				}
			}
		}
		{
			QVariant v = width();
			if(v.isValid()) {
				m_pen.setWidth(v.toReal());
			}
		}
		{
			PenStyle ps = style();
			if(ps != PenUndefined) {
				m_pen.setStyle((Qt::PenStyle)ps);
			}
		}
	}
	return m_pen;
}

QString Pen::nextSequentialName()
{
	static int n = 0;
	return QString("pen_%1").arg(++n);
}

Pen *Pen::penForName(const QString &name)
{
	Sheet *ss = reportStyleSheet();
	QF_ASSERT(ss != nullptr, "cannot find style::Sheet in parents", return nullptr);
	return ss->penForName(name);
}
