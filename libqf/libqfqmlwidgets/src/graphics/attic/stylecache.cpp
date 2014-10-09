#include "stylecache.h"

#include <qf/core/string.h>
#include <qf/core/log.h>


namespace qfc = qf::core;
using namespace qf::qmlwidgets::graphics;

const QString StyleCache::basedOnString = "basedon";
const QString StyleCache::defaultString = "default";

StyleCache::StyleCache()
{
	d = new Data();
	//qfWarning() << "new CACHE" << d.constData();
	//qfError() << QFLog::stackTrace();
}

StyleCache::~StyleCache()
{
}

StyleCache::StringMap StyleCache::splitStyleDefinition(const QString &_def)
{
	StringMap ret;
	qfc::String def = _def.trimmed();
	QStringList sl = def.splitBracketed(';', '{', '}', '\'', qfc::String::TrimParts, QString::SkipEmptyParts);
	foreach(qfc::String s, sl) {
		int ix = s.indexOf(':');
		if(ix > 0) {
			qfc::String val = s.slice(ix+1).trimmed();
			if(val[0] == '{' && val[-1] == '}') {
				val = val.slice(1, -1);
				val = val.trimmed();
			}
			ret[s.slice(0, ix)] = val;
		}
		else {
			/// 'jmeno" bez ':' znamena "basedon: jmeno"
			ret[basedOnString] = s;
		}
	}
	return ret;
}

QColor StyleCache::color(const QString &_s) const
{
	qfc::String s1 = _s.trimmed();
	if(s1.isEmpty())
		s1 = defaultString;
	if(cachedColors().contains(s1)) {
		return cachedColors().value(s1);
	}

	QColor c;// = colors().value(s);
	qfc::String def_s = definedColors().value(s1, s1);
	if(def_s == defaultString) {
		/// neexistuje defaultni definice, takze default bude defaultni cerna
		c = Qt::black;
	}
	else {
		if(def_s.startsWith("RGB(", Qt::CaseInsensitive)) {
			QStringList sl = def_s.slice(4, -1).split(',');
			QRgb rgb = 0xFF;
			foreach(QString s, sl) {
				rgb <<= 8;
				rgb += s.toInt() & 0xFF;
				//qfDebug() << "%%%%%%%%%%:" << s << (s.toInt() & 0xFF) << "#" << QString::number(rgb, 16);
			}
			c = QColor(rgb);
			//qfDebug() << "COLOR RGB:" << _s << c.name();
		}
		else if(def_s.startsWith("ARGB(", Qt::CaseInsensitive)) {
			QStringList sl = def_s.slice(5, -1).split(',');
			c = QColor(sl.value(1).toInt(), sl.value(2).toInt(), sl.value(3).toInt(), sl.value(0).toInt());
			//qfDebug() << "COLOR RGB:" << _s << c.name();
		}
		else c = QColor(def_s);
	}
	const_cast<StyleCache*>(this)->cachedColorsRef()[s1] = c;
	return c;
}

QPen StyleCache::pen(const QString &_s) const
{
	static QVector<qreal> dash_pattern;
	if(dash_pattern.isEmpty()) dash_pattern << 8 << 5;
	qfc::String s1 = _s.trimmed();
	//qfDebug() << QF_FUNC_NAME << "def:" << s1;
	if(s1.isEmpty())
		s1 = defaultString;
	if(cachedPens().contains(s1)) {
		//qfDebug() << "\tCACHED";
		return cachedPens().value(s1);
	}
	/// neni v cache, najdi definici
	qfc::String def_s = definedPens().value(s1, s1); /// bud je to nazev definovaneho pera nebo vlastni definice
	QPen p;
	if(def_s == defaultString) {
		/// neexistuje defaultni definice, takze default bude defaultni pero
	}
	else {
		StringMap map = splitStyleDefinition(def_s);
		qfc::String based_on = map.take(basedOnString);
		if(!based_on.isEmpty() && based_on != s1)
			p = pen(based_on);
		bool width_set = false;
		QMapIterator<QString, QString> i(map);
		while (i.hasNext()) {
			i.next();
			QString key = i.key().trimmed();
			qfc::String val = i.value().trimmed();
			if(!val.isEmpty()) {
				if(key == "style") {
					if(val == "solid") p.setStyle(Qt::SolidLine);
					else if(val == "dash") p.setDashPattern(dash_pattern);///p.setStyle(Qt::DashLine); prekreslovani Qt::DashLine trva dlouho pri zvetseni
					else if(val == "dot") p.setStyle(Qt::DotLine);
					else if(val == "dashdot") p.setStyle(Qt::DashDotLine);
					else if(val == "dashdotdot") p.setStyle(Qt::DashDotDotLine);
					else {
						QVector<qreal> patt;
						QStringList sl = val.splitAndTrim(',');
						foreach(QString s, sl) patt << s.toDouble();
						//foreach(double d, patt) qfInfo() << d;
						p.setDashPattern(patt);
					}
				}
				else if(key == "cap") {
					if(val == "square") p.setCapStyle(Qt::SquareCap);
					else if(val == "flat") p.setCapStyle(Qt::FlatCap);
					else if(val == "round") p.setCapStyle(Qt::RoundCap);
				}
				else if(key == "join") {
					if(val == "bevel") p.setJoinStyle(Qt::BevelJoin);
					else if(val == "miter") p.setJoinStyle(Qt::MiterJoin);
					else if(val == "round") p.setJoinStyle(Qt::RoundJoin);
				}
				else if(key == "size") {
					qreal d = val.toDouble();
					// budeme vychazet, ze QT to maji v milimetrech a my v pt
					//d = d * 25.4 / 72;
					width_set = true;
					if(d > 0) p.setWidthF(d);
				}
				else if(key == "color") {
					QColor c = color(val);
					if(c.isValid()) p.setColor(c);
				}
				else qfWarning() << "invalid pen definition attribute:" << key;
			}
		}
		if(!width_set && p.widthF() == 0) p.setWidthF(1);
	}
	qfDebug() << "\treturn width:" << p.widthF() << "color:" << p.color().name() << "style:" << p.style();
	const_cast<StyleCache*>(this)->cachedPensRef()[s1] = p;
	return p;
}

QBrush StyleCache::brush(const QString &_s) const
{
	qfc::String s1 = _s.trimmed();
	qfDebug() << QF_FUNC_NAME << "def:" << s1;
	if(s1.isEmpty())
		s1 = defaultString;
	if(cachedBrushes().contains(s1)) {
		//qfDebug() << "\tCACHED";
		return cachedBrushes().value(s1);
	}

	/// neni v cache, najdi definici
	qfc::String def_s = definedBrushes().value(s1, s1); /// bud je to nazev definovaneho pera nebo vlastni definice
	QBrush b;
	if(def_s == defaultString) {
		/// neexistuje defaultni definice, takze default bude defaultni brush
	}
	else {
		StringMap map = splitStyleDefinition(def_s);
		qfc::String based_on = map.take(basedOnString);
		if(!based_on.isEmpty() && based_on != s1)
			b = brush(based_on);
		if(!map.isEmpty()) {
			b.setStyle(Qt::SolidPattern);
			QMapIterator<QString, QString> i(map);
			while (i.hasNext()) {
				i.next();
				QString key = i.key().trimmed();
				qfc::String val = i.value().trimmed();
				if(!val.isEmpty()) {
					if(key == "style") {
						if(val == "solid") b.setStyle(Qt::SolidPattern);
						else if(val == "vertical") b.setStyle(Qt::VerPattern);
						else if(val == "horizontal") b.setStyle(Qt::HorPattern);
						else if(val == "diagonal") b.setStyle(Qt::BDiagPattern);
						else if(val == "revdiagonal") b.setStyle(Qt::FDiagPattern);
						else if(val == "cross") b.setStyle(Qt::CrossPattern);
						else if(val == "diagonalcross") b.setStyle(Qt::DiagCrossPattern);
					}
					else if(key == "color") {
						QColor c = color(val);
						if(c.isValid()) b.setColor(c);
					}
					else {
						qfWarning() << "!invalid brush definition attribute:" << key;
						b = QBrush();
					}
				}
			}
		}
	}
	qfDebug() << "\treturn brush color:" << b.color().name();
	const_cast<StyleCache*>(this)->cachedBrushesRef()[s1] = b;
	return b;
}

QFont StyleCache::font(const QString &_s) const
{
// 	qfDebug() << QF_FUNC_NAME << _s;
	qfc::String s1 = _s.trimmed();
	if(s1.isEmpty())
		s1 = defaultString;

	if(cachedFonts().contains(s1)) {
		QFont f = cachedFonts().value(s1);
		//qfDebug() << "\tCACHED" << s << ":" << f.toString();
		return f;
	}

	//qfDebug().color(QFLog::Yellow).noSpace() << QF_FUNC_NAME << " '" << s1 << "'";
	/// neni v cache, najdi definici
	qfc::String def_s = definedFonts().value(s1, s1); /// bud je to nazev definovaneho pera nebo vlastni definice
	QFont f;
	if(def_s == defaultString) {
		/// neexistuje defaultni definice, takze default bude defaultni font
	}
	else {
		StringMap map = splitStyleDefinition(def_s);
		qfc::String based_on = map.take(basedOnString);
		if(!based_on.isEmpty() && based_on != s1) {
			qfDebug() << "\t based_on:" << based_on;
			f = font(based_on);
		}
		else {
			if(!map.contains("family")) map["family"] = "Nimbus Sans L";
			if(!map.contains("size")) map["size"] = QString::number(9);
		}
		QMapIterator<QString, QString> i(map);
		while (i.hasNext()) {
			i.next();
			QString key = i.key().trimmed();
			qfc::String val = i.value().trimmed();
			qfDebug() << "\t key:" << key << "val:" << val;
			if(!val.isEmpty()) {
				if(key == "family") {
					qfDebug() << "\tset family:" << val;
					f.setFamily(val);
				}
				else if(key == "familyhint") { /// nefunguje v X11
					if(val == "sansserif") f.setStyleHint(QFont::SansSerif);
					else if(val == "serif") f.setStyleHint(QFont::Serif);
					else if(val == "typewriter") f.setStyleHint(QFont::TypeWriter);
					f.setFamily("neexistujici-family");
				}
				else if(key == "weight") {
					if(val == "light") f.setWeight(QFont::Light);
					else if(val == "normal") f.setWeight(QFont::Normal);
					else if(val == "demibold") f.setWeight(QFont::DemiBold);
					else if(val == "bold") f.setWeight(QFont::Bold);
					else if(val == "black") f.setWeight(QFont::Black);
				}
				else if(key == "style") {
					if(val == "normal") f.setStyle(QFont::StyleNormal);
					else if(val == "italic") f.setStyle(QFont::StyleItalic);
					else if(val == "oblique") f.setStyle(QFont::StyleOblique);
				}
				else if(key == "size") {
					bool incr = (val[0] == '+' || val[0] == '-');
					//if(incr) val = val.slice(1);
					qreal d = val.toDouble();
					qreal sz = 0;
					//static const double correction = 1;//0.5 * 32. / 60. * 1.2;
					if(incr) {
						//qfWarning() << "val:" << val << "d:" << d;
						sz = f.pointSizeF();
						//qfWarning() << "old size:" << sz;
						sz += d;// * correction;
						//qfWarning() << "new size:" << sz;
					}
					else {
						sz = d;// * correction;
					}
					if(sz > 0) {
						f.setPointSizeF(sz);
						qfDebug() << "\tset size:" << sz;
					}
				}
				else qfWarning() << "invalid font definition attribute:" << key;
			}
		}
	}
	//qfInfo() << "\treturn:" << s1 << "->" << f.toString();
	const_cast<StyleCache*>(this)->cachedFontsRef()[s1] = f;
	return f;
}

StyleCache::Style StyleCache::style(const QString &_s) const
{
	//if(_s == "reportheading") qfDebug().noSpace().color(QFLog::Green) << QF_FUNC_NAME << " '" << _s << "'";
	qfc::String s1 = _s.trimmed();
	if(s1.isEmpty())
		s1 = defaultString;
	Style sty;
	if(cachedStyles().contains(s1)) {
		sty = cachedStyles().value(s1);
		//qfInfo() << "CACHED:" << _s << ":" << sty.toString() << d.constData();
		return sty;
	}
/*
	if(false) {
		QMapIterator<QString, QString> i(map);
		while (i.hasNext()) {
			i.next();
			QString key = i.key().trimmed();
			qfc::String val = i.value().trimmed();
			qfDebug() << "\t [" << key << "] =" << val;
		}
	}
*/
	/// neni v cache, najdi definici
	qfc::String def_s = definedStyles().value(s1, s1); /// bud je to nazev definovaneho pera nebo vlastni definice
	//qfInfo() << "NOT CACHED:" << def_s;
	if(def_s == defaultString) {
		/// neexistuje defaultni definice, takze default bude defaultni styl
		sty.font = font(defaultString);
		sty.pen = pen(defaultString);
		sty.brush = brush(defaultString);
	}
	else {
		StringMap map = splitStyleDefinition(def_s);
		qfc::String based_on = map.take(basedOnString);
		if(!based_on.isEmpty() && based_on != s1) {
			sty = style(based_on);
			qfDebug() << "\tbase style:" << based_on << sty.toString();
			QMapIterator<QString, QString> i(map);
			while (i.hasNext()) {
				i.next();
				QString key = i.key().trimmed();
				qfc::String val = i.value().trimmed();
				if(!val.isEmpty()) {
					if(key == "pen") { sty.pen = pen(val); }
					else if(key == "brush") { sty.brush = brush(val); }
					else if(key == "font") { sty.font = font(val); }
					else qfWarning() << "invalid style definition attribute:" << key;
				}
			}
		}
		else {
			sty.font = font(map.value("font"));
			sty.pen = pen(map.value("pen"));
			sty.brush = brush(map.value("brush"));
		}
	}
	//qfInfo() << "\treturn:" << d.constData() << sty.toString();
	const_cast<StyleCache*>(this)->cachedStylesRef()[s1] = sty;
	return sty;
}

QString StyleCache::toString() const
{
	QString ret;
	QTextStream ts(&ret);
	ts << "\tdefined colors:" << endl;
	foreach(QString s, definedColors().keys()) {
		ts << "\t\t[" << s << "]" << definedColors()[s] << endl;
	}
	ts << "\tdefined pens:" << endl;
	foreach(QString s, definedPens().keys()) {
		ts << "\t\t[" << s << "]" << definedPens()[s] << endl;
	}
	ts << "\tdefined brushes:" << endl;
	foreach(QString s, definedBrushes().keys()) {
		ts << "\t\t[" << s << "]" << definedBrushes()[s] << endl;
	}
	ts << "\tdefined fonts:" << endl;
	foreach(QString s, definedFonts().keys()) {
		ts<< "\t\t[" << s << "]" << definedFonts()[s] << endl;
	}
	ts << "\tdefined styles:" << endl;
	foreach(QString s, definedStyles().keys()) {
		ts << "\t\t[" << s << "]" << definedStyles()[s] << endl;
	}

	ts << "cached colors:" << endl;
	foreach(QString s, cachedColors().keys()) {
		ts << "\t\t[" << s << "]" << cachedColors()[s].name() << endl;
	}
	ts << "cached pens:" << endl;
	foreach(QString s, cachedPens().keys()) {
		ts << "\t\t[" << s << "]" << cachedPens()[s].color().name() << endl;
	}
	ts << "cached brushes:" << endl;
	foreach(QString s, cachedBrushes().keys()) {
		ts << "\t\t[" << s << "]" << cachedBrushes()[s].color().name() << endl;
	}
	ts << "cached fonts:" << endl;
	foreach(QString s, cachedFonts().keys()) {
		ts<< "\t\t[" << s << "]" << cachedFonts()[s].toString() << endl;
	}
	ts << "cached styles:" << endl;
	foreach(QString s, cachedStyles().keys()) {
		ts << "\t\t[" << s << "]" << cachedStyles()[s].toString() << endl;
	}
	return ret;
}
/*--
static QString attrs2def(const QDomElement &el)
{
	QString def;
	QDomNamedNodeMap attrs = el.attributes();
	for(int i=0; i<attrs.count(); i++) {
		QDomAttr attr = attrs.item(i).toAttr();
		if(attr.isNull()) continue;
		if(attr.name() == "name") continue;
		def += attr.name() + ": {" + attr.value() + "};";
	}
	return def;
}

void StyleCache::readStyleSheet(const QDomElement &_el_stylesheet)
{
	qfLogFuncFrame();
	QFDomElement el_stylesheet = _el_stylesheet;
	qfc::String s;
	/// Colors
	QFDomElement el, el1;
	for(el1=el_stylesheet.firstChildElement("colors"); !!el1; el1=el1.nextSiblingElement("colors")) {
		for(el=el1.firstChildElement("color"); !!el; el=el.nextSiblingElement("color")) {
			QString key = el.attribute("name");
			if(key.isEmpty()) { qfWarning() << "empty color name"; continue; }
			qfDebug() << "\tcolor key:" << key;
			qfc::String val = el.attribute("definition");
			//QColor c = color(val);
			if(val.isEmpty()) {
				//qfWarning() << "invalid color definition name:" << key << "definition:" << val;
				continue;
			}
			/// pokud je prvni barva nepojmenovana, je defaultni
			if(definedColorsRef().isEmpty() && key != "default") definedColorsRef()["default"] = val;
			definedColorsRef()[key] = val;
		}
	}

	/// Pens
	for(el1=el_stylesheet.firstChildElement("pens"); !!el1; el1=el1.nextSiblingElement("pens")) {
		for(el=el1.firstChildElement("pen"); !!el; el=el.nextSiblingElement("pen")) {
			QString key = el.attribute("name");
			if(key.isEmpty()) { qfWarning() << "empty pen name"; continue; }
			qfDebug() << "\tpen key:" << key;
			QString def = attrs2def(el);
			//QPen p = pen(def);
			if(definedPensRef().isEmpty() && key != "default") definedPensRef()["default"] = def;
			definedPensRef()[key] = def;
		}
	}

	/// Brushes
	for(el1=el_stylesheet.firstChildElement("brushes"); !!el1; el1=el1.nextSiblingElement("brushes")) {
		for(el=el1.firstChildElement("brush"); !!el; el=el.nextSiblingElement("brush")) {
			QString key = el.attribute("name");
			if(key.isEmpty()) { qfWarning() << "empty pen name"; continue; }
			QString def = attrs2def(el);
			//QBrush b = brush(def);
			if(definedBrushesRef().isEmpty() && key != "default") definedBrushesRef()["default"] = def;
			definedBrushesRef()[key] = def;
		}
	}

	/// Fonts
	for(el1=el_stylesheet.firstChildElement("fonts"); !!el1; el1=el1.nextSiblingElement("fonts")) {
		for(el=el1.firstChildElement("font"); !!el; el=el.nextSiblingElement("font")) {
			QString key = el.attribute("name");
			if(key.isEmpty()) { qfWarning() << "empty font name"; continue; }
			QString def = attrs2def(el);
			//QFont f = font(def);
			//qfInfo() << key << "->" << def;
			if(definedFontsRef().isEmpty() && key != "default") definedFontsRef()["default"] = def;
			definedFontsRef()[key] = def;
		}
	}

	/// Styles
	for(el1=el_stylesheet.firstChildElement("styles"); !!el1; el1=el1.nextSiblingElement("styles")) {
		//qfDebug().color(QFLog::Yellow) << "\tSTYLES";
		for(el=el1.firstChildElement("style"); !!el; el=el.nextSiblingElement("style")) {
			QString key = el.attribute("name");
			if(key.isEmpty()) { qfWarning() << "empty style name"; continue; }
			QString def = attrs2def(el);
			//qfInfo() << d.constData() << key << "=" << def;
			//Style s = style(def);
			if(definedStylesRef().isEmpty() && key != "default") definedStylesRef()["default"] = def;
			//qfInfo() << key << "->" << def;
			definedStylesRef()[key] = def;
		}
	}
}

void StyleCache::readStyleSheet(style::Sheet *stylesheet)
{
	qfLogFuncFrame() << stylesheet;
	qfError() << "NIY";
}
--*/


