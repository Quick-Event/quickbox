#include "color.h"
#include "sheet.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
	StyleObject(parent, StyleObject::SGColor)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	connect(this, SIGNAL(definitionChanged(QVariant)), this, SLOT(setDirty()));
}

Color::~Color()
{
	qfLogFuncFrame() << this << "parent:" << parent();
	setName(QString());
}

QColor Color::color()
{
	if(isDirty()) {
		setDirty(false);
		QVariant v = definition();
		//qfInfo() << v.toString() << "type:" << v.typeName();
#if QT_VERSION_MAJOR >= 6
		if(v.typeId() == QMetaType::QString) {
#else
		if(v.type() == QVariant::String) {
#endif
			m_color = QColor::fromString(v.toString());
			if(!m_color.isValid()) {
				Sheet *ss = rootStyleSheet();
				if(ss) {
					Color *pco = qobject_cast<Color*>(ss->styleObjectForName(SGColor, v.toString()));
					if(pco)
						m_color = pco->color();
				}
			}
		}
#if QT_VERSION_MAJOR >= 6
		else if(v.typeId() == QMetaType::Int) {
#else
		else if(v.type() == QVariant::Int) {
#endif
			m_color = QColor((Qt::GlobalColor)v.toInt());
		}
		else {
			m_color = v.value<QColor>();
		}
	}
	return m_color;
}
