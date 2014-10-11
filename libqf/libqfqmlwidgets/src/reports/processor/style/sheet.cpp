#include "sheet.h"

using namespace qf::qmlwidgets::reports::style;

//==========================================================
//           Sheet
//==========================================================
Sheet::Sheet(QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
}

Sheet::~Sheet()
{
	qfLogFuncFrame();
	//qDeleteAll(m_colors); it seems that QQmlListProperty itself handles color children parentship
}

void Sheet::createStyleCache()
{
	m_definedStyles.clear();
	createStyleCache_helper(this);
}

void Sheet::createStyleCache_helper(QObject *parent)
{
	if(!parent)
		return;
	for(auto child : parent->children()) {
		StyleObject *so = dynamic_cast<StyleObject*>(child);
		if(so) {
			QString name = so->name();
			if(!name.isEmpty()) {
				setStyleObjectForName(so->styleGroup(), name, so);
			}
		}
		createStyleCache_helper(child);
	}
}

QObject *Sheet::styleObjectForName(StyleObject::StyleGroup style_object_group, const QString &name, bool should_exist)
{
    ObjectMap om = m_definedStyles.value(style_object_group);
    QObject *ret = om.value(name);
    if(ret == nullptr && should_exist)
        qfError() << "Cannot find style object for style group:" << style_object_group << "name:" + name;
    return ret;
}

void Sheet::setStyleObjectForName(StyleObject::StyleGroup style_object_group, const QString &name, QObject *o)
{
	//qfInfo() << Q_FUNC_INFO << name << style_object_group << o;
    ObjectMap &om = m_definedStyles[style_object_group];
    if(o)
        om[name] = o;
	else
        om.remove(name);
}

QQmlListProperty<Color> Sheet::colors()
{
	/// Generally this constructor should not be used in production code, as a writable QList violates QML's memory management rules. However, this constructor can be very useful while prototyping.
	return QQmlListProperty<Color>(this, m_colors);
}

QQmlListProperty<Pen> Sheet::pens()
{
	return QQmlListProperty<Pen>(this, m_pens);
}

QQmlListProperty<Brush> Sheet::brushes()
{
	return QQmlListProperty<Brush>(this, m_brushes);
}

QQmlListProperty<Font> Sheet::fonts()
{
	return QQmlListProperty<Font>(this, m_fonts);
}

QQmlListProperty<Text> Sheet::textStyles()
{
	return QQmlListProperty<Text>(this, m_textStyles);
}

