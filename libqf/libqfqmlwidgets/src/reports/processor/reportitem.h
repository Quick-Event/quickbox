//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEM_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEM_H

#include "../../qmlwidgetsglobal.h"
#include "../../graphics/graphics.h"

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
class ReportItemDetail;
class ReportItemMetaPaintFrame;

//! Base class of report elements.
class QFQMLWIDGETS_DECL_EXPORT ReportItem : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
private:
	typedef QObject Super;
public:
	Q_PROPERTY(bool keepAll READ isKeepAll WRITE setKeepAll NOTIFY keepAllChanged)
	Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
public:
	enum Layout {LayoutInvalid = graphics::LayoutInvalid,
				 LayoutHorizontal = graphics::LayoutHorizontal,
				 LayoutVertical = graphics::LayoutVertical,
				 LayoutStack = graphics::LayoutStack
				};
	//typedef graphics::Layout Layout;
	Q_ENUMS(Layout)
public:
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
private:
	bool m_visible;
public:
	ReportItem(ReportItem *parent);
	~ReportItem() Q_DECL_OVERRIDE;
public:
	typedef QDomElement HTMLElement;
	static const double Epsilon;
	static const QString INFO_IF_NOT_FOUND_DEFAULT_VALUE;
	//typedef Layout Layout;
	enum PrintResultValue {
		PrintNotPrintedYet = 0,
		PrintOk = 1, ///< tisk se zdaril
		PrintNotFit ///< nevytiskl se item nebo vsechny jeho deti, zalomi se stranka/sloupec se a zkusi se to znovu
	};
	enum PrintResultFlags {
		FlagNone = 0,
		FlagPrintAgain = 1, ///< detail se sice vesel, ale protoze data obsahuji dalsi radky, tiskni ho dal, pouziva se s PrintOk.
		//FlagPrintWidthNotFit = 2, ///< Text se nevesel na sirku, takze se neda nic delat, pouziva se s PrintNotFit
		FlagPrintNeverFit = 4, ///< tisk se nepodaril a nikdy se nepodari, pouziva se s PrintNotFit
		FlagPrintBreak = 8 ///< PrintNotFit je protoze je page nebo column break
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
			LayoutVerticalFlag = 8, /// rect ma layout ve smeru y, pokud je kombinace LayoutX a LayoutY nesmyslna predpoklada se LayoutX == 0 LayoutY == 1
			BackgroundItem = 16
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
			if(ly == LayoutHorizontal) setWidth(sz);
			else setHeight(sz);
			return *this;
		}
		Rect& cutSizeInLayout(const Rect &rect, Layout ly)
		{
			if(ly == LayoutHorizontal) {
				if(rect.right() > right()) setLeft(right());
				else setLeft(rect.right());
			}
			else {
				if(rect.bottom() > bottom()) setTop(bottom());
				else setTop(rect.bottom());
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
			if(flags & ExpandChildrenFrames) ret += 'E';
			if(flags & LayoutHorizontalFlag) ret += 'X';
			if(flags & LayoutVerticalFlag) ret += 'Y';
			if(flags & BackgroundItem) ret += '^';
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
		qreal size;
		Rect::Unit unit;

		double fillLayoutRatio() const {return (unit == Rect::UnitPercent)? size / 100.: -1;}
		ChildSize(qreal s = 0, Rect::Unit u = Rect::UnitMM) : size(s), unit(u) {}
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
	virtual  ReportItemBand* toBand()  {return NULL;}
	virtual  ReportItemDetail* toDetail()  {return NULL;}
	//! vrati band, ktery item obsahuje nebo NULL
	virtual ReportItemBand* parentBand();
protected:
	qf::core::utils::TreeTable findDataTable(const QString &name);
protected:
	//! vrati detail, ktery item obsahuje nebo item, pokud je item typu detail nebo NULL.
	virtual ReportItemDetail* currentDetail();

	QVariant value(const QString &data_src, const QString &domain = "row", const QVariantList &params = QVariantList(), const QVariant &default_value = ReportItem::INFO_IF_NOT_FOUND_DEFAULT_VALUE, bool sql_match = true);
	/// poukud ma node jen jedno dite vrati to jeho hodnotu vcetne typu, pokud je deti vic, udela to z nich jeden string
	//--QVariant concatenateNodeChildrenValues(const QDomNode &nd) ;
	//--QString nodeText(const QDomNode &nd) ;
	//--QVariant nodeValue(const QDomNode &nd) ;

	//! Pokud byl predchozi result PrintNotFit a soucasny opet PrintNotFit, znamena to, ze se item uz nikdy nevejde,
	//! zavedenim tohoto fieldu zabranim nekonecnemu odstrankovavani.
	PrintResult checkPrintResult(PrintResult res);
	/*--
	void updateChildren() {
		if(!childrenSynced()) { syncChildren(); }
	}
	virtual bool childrenSynced();
	virtual void syncChildren();
	void deleteChildren();
	--*/
public:
	//! Vraci atribut elementu itemu.
	//! Pokud hodnota \a attr_name je ve tvaru 'script:funcname', zavola se scriptDriver processoru, jinak se vrati atribut.
	QString elementAttribute(const QString &attr_name, const QString &default_val = QString());

	ReportItem* parent() const {return static_cast<ReportItem*>(this->Super::parent());}
	//--virtual ReportItem* childAt(int ix) const {return static_cast<ReportItem*>(this->children()[ix]);}
	//! Print item in form, that understandable by ReportPainter.
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect) = 0;
	//! Print item in HTML element form.
	virtual PrintResult printHtml(HTMLElement &out) {Q_UNUSED(out); return PrintOk;}
	/// vrati definovanou velikost pro item a layout
	virtual ChildSize childSize(Layout parent_layout) {Q_UNUSED(parent_layout); return ChildSize();}

	ReportItemFrame* parentFrame() const
	{
		if(parent()) return parent()->toFrame();
		return NULL;
	}
	static const bool IncludingParaTexts = true;
	/// nekdy je potreba jen dotisknout texty a ramecky vytisknout znova, pak je \a including_para_texts == false
	virtual void resetIndexToPrintRecursively(bool including_para_texts) {Q_UNUSED(including_para_texts);}
	virtual bool isBreak() {return false;}

	virtual QString toString(int indent = 2, int indent_offset = 0);

	ReportItemMetaPaint* createMetaPaintItem(ReportItemMetaPaint *parent);
	/*--
	virtual ReportItem* cd(const qf::core::utils::TreeItemPath &path) const {
		return dynamic_cast<ReportItem*>(Super::cd(path));
	}
	--*/
protected:
	virtual void componentComplete() {}
public:
	ReportProcessor *processor;
	//--QDomElement element;
	Rect designedRect;

	bool recentlyPrintNotFit;
	//PrintResult recentPrintResult;
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBreak : public ReportItem
{
	Q_OBJECT
private:
	typedef ReportItem Super;
protected:
	bool breaking;
public:
	virtual bool isBreak() {return true;}

	virtual ChildSize childSize(Layout parent_layout) {
		Q_UNUSED(parent_layout);
		return ChildSize(0, Rect::UnitInvalid);
	}
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
public:
	ReportItemBreak(ReportItem *parent);
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemFrame : public ReportItem
{
	Q_OBJECT
private:
	typedef ReportItem Super;
public:
	Q_PROPERTY(QQmlListProperty<ReportItem> items READ items)
	Q_CLASSINFO("DefaultProperty", "items")
	Q_ENUMS(Alignment)
	//Q_PROPERTY(qreal x1 READ x1 WRITE setX1 NOTIFY x1Changed)
	//Q_PROPERTY(qreal x2 READ x2 WRITE setX2 NOTIFY x2Changed)
	//Q_PROPERTY(qreal y1 READ y1 WRITE setY1 NOTIFY y1Changed)
	//Q_PROPERTY(qreal y2 READ y2 WRITE setY2 NOTIFY y2Changed)
	Q_PROPERTY(qreal hinset READ hinset WRITE setHinset NOTIFY hinsetChanged)
	Q_PROPERTY(qreal vinset READ vinset WRITE setVinset NOTIFY vinsetChanged)
	Q_PROPERTY(QString width READ width WRITE setWidth NOTIFY widthChanged)
	Q_PROPERTY(QString height READ height WRITE setHeight NOTIFY heightChanged)
	Q_CLASSINFO("property.layout.doc",
				"LayoutHorizontal - place children in the row\n"
				"LayoutVertical - place children in the column\n"
				"LayoutStack - place all children on stack"
				)
	Q_PROPERTY(Layout layout READ layout WRITE setLayout NOTIFY layoutChanged)
	Q_CLASSINFO("property.expandChildrenFrames.doc",
				"ramecky deti, jsou roztazeny tak, aby vyplnily parent frame, "
				"jinymi slovy, pokud v radku tabulky natece kazde policko jinak vysoke, budou vsechny roztazeny na vysku parent frame.\n"
				"Natahuji se jen ramecky, poloha vyrendrovaneho obsahu zustava nezmenena, "
				"dela se to tak, ze se nejprve vyrendruje stranka a pak se prochazi vyrendrovane ramecky a pokud je treba, "
				"zvetsuji se tak, aby vyplnily cely parent frame.  Objekty typu QFReportItemMetaPaintText jsou ignorovany"
				)
	Q_PROPERTY(bool expandChildrenFrames READ isExpandChildrenFrames WRITE setExpandChildrenFrames NOTIFY expandChildrenFramesChanged)
	Q_PROPERTY(Alignment halign READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
	Q_PROPERTY(Alignment valign READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)
public:
	enum Alignment {
		AlignLeft = Qt::AlignLeft,
		AlignRight = Qt::AlignRight,
		AlignCenter = Qt::AlignHCenter,
		AlignTop = Qt::AlignTop,
		AlignBottom = Qt::AlignBottom
	};
	//QF_PROPERTY_IMPL(qreal, x, X, 1)
	//QF_PROPERTY_IMPL(qreal, y, Y, 1)
	//QF_PROPERTY_IMPL(qreal, x, X, 2)
	//QF_PROPERTY_IMPL(qreal, y, Y, 2)
	QF_PROPERTY_IMPL(qreal, h, H, inset)
	QF_PROPERTY_IMPL(qreal, v, V, inset)
	QF_PROPERTY_IMPL(QString, w, W, idth)
	QF_PROPERTY_IMPL(QString, h, H, eight)
	QF_PROPERTY_IMPL(Layout, l, L, ayout)
	QF_PROPERTY_BOOL_IMPL(e, E, xpandChildrenFrames)
	QF_PROPERTY_IMPL(Alignment, h, H, orizontalAlignment)
	QF_PROPERTY_IMPL(Alignment, v, V, erticalAlignment)
public:
	ReportItemFrame(ReportItem *parent);
	~ReportItemFrame() Q_DECL_OVERRIDE {}
public:
	bool isRubber(Layout ly) {
		ChildSize sz = childSize(ly);
		return (sz.size == 0 && sz.unit == Rect::UnitMM);
	}
	// list obsahujici rozmery deti ve frame
	//QList<qreal> childrenSizesInLayout;
	//QList<qreal> childrenSizesInOrthogonalLayout;
	//! children, kterym se ma zacit pri tisku
	int indexToPrint;

	static Layout orthogonalLayout(Layout l) {
		if(l == LayoutHorizontal)
			return LayoutVertical;
		if(l == LayoutVertical)
			return LayoutHorizontal;
		return LayoutInvalid;
	}
	Layout orthogonalLayout() const {return orthogonalLayout(layout());}
protected:
	ChildSize childSize(Layout parent_layout) Q_DECL_OVERRIDE;
	virtual ReportItemFrame* toFrame() {return this;}

	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
	//! Nastavi u sebe a u deti indexToPrint na nulu, aby se vytiskly na dalsi strance znovu.
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
	Layout parentLayout() const
	{
		ReportItemFrame *frm = parentFrame();
		if(!frm)
			return LayoutInvalid;
		return frm->layout();
	}

	void componentComplete() Q_DECL_OVERRIDE;
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printHtml(HTMLElement &out);

	//--const QList<double>& gridLayoutSizes() {return f_gridLayoutSizes;}
	//--void setGridLayoutSizes(const QList<double> &szs) {f_gridLayoutSizes = szs;}

	QString toString(int indent = 2, int indent_offset = 0) Q_DECL_OVERRIDE;

	QQmlListProperty<ReportItem> items();
private:
	static void addItemFunction(QQmlListProperty<ReportItem> *list_property, ReportItem *item);
	static ReportItem* itemAtFunction(QQmlListProperty<ReportItem> *list_property, int index);
	static void removeAllItemsFunction(QQmlListProperty<ReportItem> *list_property);
	static int countItemsFunction(QQmlListProperty<ReportItem> *list_property);
protected:
	int itemCount() const;
	ReportItem* itemAt(int index);
private:
	QList<ReportItem*> m_items;
	//--QList<double> f_gridLayoutSizes;
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBand : public ReportItemFrame
{
	Q_OBJECT
private:
	typedef ReportItemFrame Super;
protected:
	qf::core::utils::TreeTable f_dataTable;
	// pokud obsah f_dataTable nepochazi z dat, ale nahraje se dynamicky pomoci elementu <data domain="sql">, ulozi se sem dokument, ktery data z tabulky drzi
	//QFXmlTableDocument f_dataTableOwnerDocument;
	bool dataTableLoaded;
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	virtual  ReportItemBand* toBand()  {return this;}
	ReportItemDetail* detail();

	virtual qf::core::utils::TreeTable dataTable();
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
public:
	ReportItemBand(ReportItem *parent);
	virtual ~ReportItemBand() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemDetail : public ReportItemFrame
{
	Q_OBJECT
	friend class ReportItemBand;
private:
	typedef ReportItemFrame Super;
protected:
	//qf::core::utils::TreeTableRow f_dataRow;
	int f_currentRowNo;

	virtual ReportItemDetail* toDetail() {return this;}
public:
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printHtml(HTMLElement &out);

	qf::core::utils::TreeTable dataTable();
	qf::core::utils::TreeTableRow dataRow();
	/// cislo prave tisteneho radku, pocitano od nuly.
	int currentRowNo() const {return f_currentRowNo;}
	void resetCurrentRowNo() {f_currentRowNo = 0;}
public:
	ReportItemDetail(ReportItem *parent);
	virtual ~ReportItemDetail() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemReport : public ReportItemBand
{
	Q_OBJECT
private:
	typedef ReportItemBand Super;
protected:
	/// body a report ma tu vysadu, ze se muze vickrat za sebou nevytisknout a neznamena to print forever.
	//virtual PrintResult checkPrintResult(PrintResult res) {return res;}
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	virtual qf::core::utils::TreeTable dataTable();
public:
	ReportItemReport(QObject *parent);
	virtual ~ReportItemReport() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBody : public ReportItemDetail
{
	Q_OBJECT
private:
	typedef ReportItemDetail Super;
protected:
	/// body a report ma tu vysadu, ze se muze vickrat za sebou nevytisknout a neznamena to print forever.
	//virtual PrintResult checkPrintResult(PrintResult res) {return res;}
public:
	ReportItemBody(ReportItem *parent)
		: Super(parent) {}
	virtual ~ReportItemBody() {}

	//virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
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
//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemPara : public ReportItemFrame
{
	Q_OBJECT
private:
	typedef ReportItemFrame Super;
protected:
	/// tiskne se printed text od indexToPrint, pouziva se pouze v pripade, ze text pretece na dalsi stranku
	QString printedText;
	QTextLayout textLayout;
protected:
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
	QString paraStyleDefinition();
	QString paraText();
public:
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printHtml(HTMLElement &out);
public:
	ReportItemPara(ReportItem *parent);
	virtual ~ReportItemPara() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemImage : public ReportItemFrame
{
	Q_OBJECT
private:
	typedef ReportItemFrame Super;
protected:
	QString src;
	bool childrenSyncedFlag;
	//--QDomElement fakeLoadErrorPara;
	//--QDomDocument fakeLoadErrorParaDocument;
protected:
	virtual bool childrenSynced();
	virtual void syncChildren();
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
public:
	ReportItemImage(ReportItem *parent)
		: Super(parent), childrenSyncedFlag(false) {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemGraph : public ReportItemImage
{
	Q_OBJECT
private:
	typedef ReportItemImage Super;
protected:
	virtual void syncChildren();
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
public:
	ReportItemGraph(ReportItem *parent)
		: Super(parent) {}
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEM_H

