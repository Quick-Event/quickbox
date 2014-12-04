//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEM_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEM_H

#include "../../qmlwidgetsglobal.h"
#include "../../graphics/graphics.h"
#include "style/pen.h"
#include "style/brush.h"
#include "style/text.h"
#include "style/sheet.h"

#include <qf/core/utils.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/utils/treeitembase.h>

#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QTextLayout>
#include <QPicture>
#include <QQmlParserStatus>
#include <QQmlListProperty>

class QDomElement;
class QDomText;

namespace qf {
namespace qmlwidgets {
namespace reports {

class ReportProcessor;
class ReportItemMetaPaint;

class ReportItemFrame;
class ReportItemBand;
class ReportItemMetaPaintFrame;

//! Base class of report elements.
class QFQMLWIDGETS_DECL_EXPORT ReportItem : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_ENUMS(Layout)
	Q_PROPERTY(bool keepAll READ isKeepAll WRITE setKeepAll NOTIFY keepAllChanged)
	Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
private:
	typedef QObject Super;
public:
	ReportItem(ReportItem *parent = nullptr);
	~ReportItem() Q_DECL_OVERRIDE;
public:
	enum Layout {LayoutInvalid = graphics::LayoutInvalid,
				 LayoutHorizontal = graphics::LayoutHorizontal,
				 LayoutVertical = graphics::LayoutVertical,
				 LayoutStacked = graphics::LayoutStacked
				};
	//typedef graphics::Layout Layout;

	/// Pokud ma frame keepAll atribut a dvakrat za sebou se nevytiskne, znamena to, ze se nevytiskne uz nikdy.
	QF_PROPERTY_BOOL_IMPL(k, K, eepAll)
	Q_INVOKABLE bool isVisible();
	Q_SLOT void setVisible(bool b) {
		if(m_visible != b) {
			m_visible = b;
			emit visibleChanged(m_visible);
		}
	}
	Q_SIGNAL void visibleChanged(bool new_val);
public:
	typedef QDomElement HTMLElement;
	static const double Epsilon;
	static const QString INFO_IF_NOT_FOUND_DEFAULT_VALUE;
	//typedef Layout Layout;
	enum PrintResultValue {
		PrintNotPrintedYet = 0,
		PrintOk = 1, ///< printed successfully
		PrintNotFit ///< partialy printed, insert new page to document and continue
	};
	enum PrintResultFlags {
		FlagNone = 0,
		FlagPrintAgain = 1, ///< detail se sice vesel, ale protoze data obsahuji dalsi radky, tiskni ho dal, pouziva se s PrintOk.
		FlagPrintNeverFit = 4, ///< tisk se nepodaril a nikdy se nepodari, pouziva se s PrintNotFit
		//FlagPrintBreak = 8 ///< PrintNotFit is caused by page or column break
	};
	struct PrintResult
	{
		quint16 value;
		quint16 flags;
		PrintResult() : value(0), flags(0) {}
		PrintResult(PrintResultValue val, PrintResultFlags f = FlagNone) : value((quint16)val), flags((quint16)f) {}
		QString toString() const {return QString("value: %1 flags: %2")
					.arg(value==PrintOk? "OK": value==PrintNotFit? "NotFit": "NotPrintedYet")
					.arg(QString((flags & FlagPrintAgain)? "PrintAgain ": " ")) + ((flags & FlagPrintNeverFit)? "PrintNeverFit ": " ");}
	};
public:
	typedef graphics::Point Point;
	class Size : public graphics::Size
	{
	public:
		Size() : graphics::Size(0, 0) {}
		Size(qreal x, qreal y) : graphics::Size(x, y) {}
		Size(const graphics::Size &s) : graphics::Size(s) {}
		Size(const QSizeF &s) : graphics::Size(s) {}

		qreal sizeInLayout(Layout ly)
		{
			if(ly == LayoutHorizontal)
				return width();
			return height();
		}

		Size& addSizeInLayout(const Size &sz, Layout ly)
		{
			if(ly == LayoutHorizontal) {
				setWidth(width() + sz.width());
				setHeight(qMax(height(), sz.height()));
			}
			else {
				setHeight(height() + sz.height());
				setWidth(qMax(width(), sz.width()));
			}
			return *this;
		}
	};
	class Rect : public graphics::Rect
	{
	public:
		enum Flag {
			//LeftFixed = 1,
			//TopFixed = 2,
			//BottomFixed = 4,
			//RightFixed = 8,
			FillLayout = 1, /// tento item se natahne ve smeru layoutu tak, aby vyplnil cely bounding_rect
			ExpandChildrenFrames = 2, /// viz. atribut expandChildrenFrames v Report.rnc
			LayoutHorizontalFlag = 4, /// rect ma layout ve smeru x
			LayoutVerticalFlag = 8 /// rect ma layout ve smeru y, pokud je kombinace LayoutX a LayoutY nesmyslna predpoklada se LayoutX == 0 LayoutY == 1
			//--BackgroundItem = 16
		};
		enum Unit {UnitInvalid = 0, UnitMM, UnitPercent};
	public:
		unsigned flags;
		Unit horizontalUnit, verticalUnit;
	public:
		/*--
		bool isAnchored()  const
		{
			return flags & (LeftFixed | TopFixed | BottomFixed | RightFixed);
		}
		--*/
		bool isRubber(Layout ly)  const
		{
			if(ly == LayoutHorizontal)
				return (width() == 0 && horizontalUnit == UnitMM);
			return (height() == 0 && verticalUnit == UnitMM);
		}
		Unit unit(Layout ly)  const
		{
			if(ly == LayoutHorizontal)
				return horizontalUnit;
			return verticalUnit;
		}

		qreal sizeInLayout(Layout ly) const
		{
			return Size(size()).sizeInLayout(ly);
		}
		Rect& setSizeInLayout(qreal sz, Layout ly)
		{
			if(ly == LayoutHorizontal)
				setWidth(sz);
			else
				setHeight(sz);
			return *this;
		}
		Rect& cutSizeInLayout(const Rect &rect, Layout ly)
		{
			if(ly == LayoutHorizontal) {
				if(rect.right() > right())
					setLeft(right());
				else
					setLeft(rect.right());
			}
			else {
				if(rect.bottom() > bottom())
					setTop(bottom());
				else
					setTop(rect.bottom());
			}
			return *this;
		}

		QString toString() const {
			return QString("rect(%1, %2, size[%3%4, %5%6]) [%7]")
					.arg(Point(topLeft()).toString(), Point(bottomRight()).toString())
					.arg(size().width()).arg(unitToString(horizontalUnit))
					.arg(size().height()).arg(unitToString(verticalUnit))
					.arg(flagsToString(flags));
		}

		static QString unitToString(Unit u) {
			QString ret;
			switch(u) {
			case UnitMM:
				ret = "mm"; break;
			case UnitPercent:
				ret = "%"; break;
			default:
				ret = "invalid";
			}
			return ret;
		}

		static QString flagsToString(unsigned flags) {
			QString ret;
			//if(flags & LeftFixed) ret += 'L';
			//if(flags & TopFixed) ret += 'T';
			//if(flags & RightFixed) ret += 'R';
			//if(flags & BottomFixed) ret += 'B';
			if(flags & FillLayout) ret += 'F';
			if(flags & ExpandChildrenFrames) ret += 'X';
			if(flags & LayoutHorizontalFlag) ret += 'H';
			if(flags & LayoutVerticalFlag) ret += 'V';
			//--if(flags & BackgroundItem) ret += '^';
			return ret;
		}
	private:
		void init() {
			flags = 0;
			horizontalUnit = verticalUnit = UnitMM;
		}
	public:
		Rect() : graphics::Rect() {init();}
		//Rect(qreal x, qreal y) : QRectF(x, y) {init();}
		Rect(const QPointF &topLeft, const QSizeF &size) : graphics::Rect(topLeft, size) {init();}
		Rect(qreal x, qreal y, qreal width, qreal height) : graphics::Rect(x, y, width, height) {init();}
		Rect(const QRectF &r) : graphics::Rect(r) {init();}
		Rect(const graphics::Rect &r) : graphics::Rect(r) {init();}
	};
public:
	struct ChildSize {

		ChildSize(qreal s = 0, Rect::Unit u = Rect::UnitMM) : size(s), unit(u) {}

		double fillLayoutRatio() const
		{
			return (unit == Rect::UnitPercent)? size / 100.: -1;
		}

		double fromParentSize(double parent_size) const
		{
			double ret = parent_size;
			if(unit == Rect::UnitMM) {
				if(size > 0)
					ret = size;
			}
			else if(unit == Rect::UnitPercent) {
				if(size > 0)
					ret = size / 100 * ret;
			}
			return ret;
		}

		qreal size;
		Rect::Unit unit;
	};
public:
	struct Image
	{
		QPicture picture;
		//QPixmap pixmap;
		QImage image;

		//bool isPixmap() const {return !pixmap.isNull();}
		bool isPicture() const {return !picture.isNull();}
		bool isImage() const {return !image.isNull();}
		Size size() const {
			if(isImage()) return QSizeF(image.size());
			//if(isPixmap()) return pixmap.size();
			if(isPicture()) return QSizeF(picture.boundingRect().size());
			return Size();
		}
		bool isNull() const {return picture.isNull() && image.isNull();}
		//const QImage& toQImage() const {return image;}

		Image() {}
		//Image(const QPixmap &px) : pixmap(px) {}
		Image(const QPicture &pc) : picture(pc) {}
		Image(const QImage &im) : image(im) {}
	};
public:
	virtual ReportItemFrame* toFrame() {return NULL;}
protected:
	//--qf::core::utils::TreeTable findDataTable(const QString &name);
protected:
	ReportItemBand *parentBand();

	QVariant value(const QString &data_src, const QString &domain = "row", const QVariantList &params = QVariantList(), const QVariant &default_value = ReportItem::INFO_IF_NOT_FOUND_DEFAULT_VALUE, bool sql_match = true);
	/// poukud ma node jen jedno dite vrati to jeho hodnotu vcetne typu, pokud je deti vic, udela to z nich jeden string

	//! Pokud byl predchozi result PrintNotFit a soucasny opet PrintNotFit, znamena to, ze se item uz nikdy nevejde,
	//! zavedenim tohoto fieldu zabranim nekonecnemu odstrankovavani.
	PrintResult checkPrintResult(PrintResult res);
public:
	ReportProcessor* processor(bool throw_exc = qf::core::Exception::Throw);
	//! Vraci atribut elementu itemu.
	//! Pokud hodnota \a attr_name je ve tvaru 'script:funcname', zavola se scriptDriver processoru, jinak se vrati atribut.
	//--QString elementAttribute(const QString &attr_name, const QString &default_val = QString());

	ReportItem* parent() const {
		return static_cast<ReportItem*>(this->Super::parent());
	}
	//! Print item in form, that understandable by ReportPainter.
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect) {Q_UNUSED(out); Q_UNUSED(bounding_rect); return PrintOk;}
	//! Print item in HTML element form.
	virtual PrintResult printHtml(HTMLElement &out) {Q_UNUSED(out); return PrintOk;}
	/// vrati definovanou velikost pro item a layout
	virtual ChildSize childSize(Layout parent_layout) {Q_UNUSED(parent_layout); return ChildSize();}

	ReportItemFrame* parentFrame() const
	{
		if(parent())
			return parent()->toFrame();
		return NULL;
	}
	static const bool IncludingParaTexts = true;
	/// nekdy je potreba jen dotisknout texty a ramecky vytisknout znova, pak je \a including_para_texts == false
	virtual void resetIndexToPrintRecursively(bool including_para_texts) {Q_UNUSED(including_para_texts);}
	virtual bool isBreak() {return false;}

	virtual QString toString(int indent = 2, int indent_offset = 0);

protected:
	style::Text* effectiveTextStyle();

	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
public:
	Rect designedRect;
protected:
	bool m_recentlyPrintNotFit;
private:
	bool m_visible;
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBreak : public ReportItem
{
	Q_OBJECT
private:
	typedef ReportItem Super;
public:
	ReportItemBreak(ReportItem *parent = nullptr);
public:
	virtual bool isBreak() {return true;}

	virtual ChildSize childSize(Layout parent_layout) {
		Q_UNUSED(parent_layout);
		return ChildSize(0, Rect::UnitInvalid);
	}
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
private:
	bool m_breaking;
};


/*--
//! TODO: write class documentation.
class ReportItemTable : public ReportItemBand
{
	Q_OBJECT
private:
	typedef ReportItemBand Super;
protected:
	QDomDocument fakeBandDocument;
	QDomElement fakeBand;
	void createFakeBand();
	virtual bool childrenSynced() {return children().count() > 0;}
	virtual void syncChildren();
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
public:
	ReportItemTable(ReportItem *parent);
	virtual ~ReportItemTable() {}
};
--*/

#ifdef REPORT_ITEM_GRAPH
//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemGraph : public ReportItemImage
{
	Q_OBJECT
private:
	typedef ReportItemImage Super;
protected:
	//--virtual void syncChildren();
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
public:
	ReportItemGraph(ReportItem *parent = nullptr)
		: Super(parent) {}
};
#endif
}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEM_H

