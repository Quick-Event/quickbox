#ifndef QF_QMLWIDGETS_GRAPHICS_STYLECACHE_H
#define QF_QMLWIDGETS_GRAPHICS_STYLECACHE_H

#include "../qmlwidgetsglobal.h"

#include <QMap>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QSharedData>

class QDomElement;

namespace qf {
namespace qmlwidgets {
namespace graphics {

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT StyleCache
{
	protected:
		static const QString basedOnString;
		static const QString defaultString;
	public:
		typedef QMap<QString, QString> StringMap;
		typedef QMap<QString, QPen> PenMap;
		typedef QMap<QString, QBrush> BrushMap;
		typedef QMap<QString, QFont> FontMap;
		struct Style {
			QPen pen; QBrush brush; QFont font;

			QString toString() const
			{
				QString ret;
				ret += "font: " + font.toString();
				ret += " pen: " + pen.color().name();
				ret += " brush: " + brush.color().name();
				return ret;
			}
		};
		typedef QMap<QString, Style> StyleMap;
		typedef QMap<QString, QColor> ColorMap;
	private:
		struct StyleSheetDefinition
		{
			StringMap colors;
			StringMap pens;
			StringMap brushes;
			StringMap fonts;
			StringMap styles;
		};
		struct StyleSheetCache
		{
			ColorMap colors;
			PenMap pens;
			BrushMap brushes;
			FontMap fonts;
			StyleMap styles;
		};
		struct Data : public QSharedData
		{
			StyleSheetDefinition defined;
			StyleSheetCache cached;
		};
		QExplicitlySharedDataPointer<Data> d;
	protected:
		static StringMap splitStyleDefinition(const QString &def);

		const StringMap& definedColors() const {return d->defined.colors;}
		const StringMap& definedPens() const {return d->defined.pens;}
		const StringMap& definedBrushes() const {return d->defined.brushes;}
		const StringMap& definedFonts() const {return d->defined.fonts;}
		const StringMap& definedStyles() const {return d->defined.styles;}

		const ColorMap& cachedColors() const {return d->cached.colors;}
		const PenMap& cachedPens() const {return d->cached.pens;}
		const BrushMap& cachedBrushes() const {return d->cached.brushes;}
		const FontMap& cachedFonts() const {return d->cached.fonts;}
		const StyleMap& cachedStyles() const {return d->cached.styles;}

		StringMap& definedColorsRef() {return d->defined.colors;}
		StringMap& definedPensRef() {return d->defined.pens;}
		StringMap& definedBrushesRef() {return d->defined.brushes;}
		StringMap& definedFontsRef() {return d->defined.fonts;}
		StringMap& definedStylesRef() {return d->defined.styles;}
	
		ColorMap& cachedColorsRef() {return d->cached.colors;}
		PenMap& cachedPensRef() {return d->cached.pens;}
		BrushMap& cachedBrushesRef() {return d->cached.brushes;}
		FontMap& cachedFontsRef() {return d->cached.fonts;}
		StyleMap& cachedStylesRef() {return d->cached.styles;}
	public:
		QColor color(const QString &_s) const;
		QPen pen(const QString &_s) const;
		QBrush brush(const QString &_s) const;
		QFont font(const QString &_s) const;
		Style style(const QString &_s) const;

		void readStyleSheet(const QDomElement &el_stylesheet);
		void readStyleSheet(QObject *stylesheet);
		/// predefinovanim neceho a zmazani cache zpusobi, ze nove NECO se projevi ve vsech odvozenych definicich
		void clearCache() {d->cached = StyleSheetCache();}

		QString toString() const;
	public:
		StyleCache();
		virtual ~StyleCache();
};

}}}

#endif // QF_QMLWIDGETS_GRAPHICS_STYLECACHE_H

