//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
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
class ReportItemDetail;
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
				 LayoutStack = graphics::LayoutStack
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
	ReportProcessor* processor();
	//! Vraci atribut elementu itemu.
	//! Pokud hodnota \a attr_name je ve tvaru 'script:funcname', zavola se scriptDriver processoru, jinak se vrati atribut.
	//--QString elementAttribute(const QString &attr_name, const QString &default_val = QString());

	ReportItem* parent() const {
		return static_cast<ReportItem*>(this->Super::parent());
	}
	//--virtual ReportItem* childAt(int ix) const {return static_cast<ReportItem*>(this->children()[ix]);}
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

	/*--
	virtual ReportItem* cd(const qf::core::utils::TreeItemPath &path) const {
		return dynamic_cast<ReportItem*>(Super::cd(path));
	}
	--*/
protected:
	style::Text* effectiveTextStyle();
	//virtual void setupMetaPaintItem(ReportItemMetaPaint *mpit);

	void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;
public:
	//--QDomElement element;
	Rect designedRect;

	bool recentlyPrintNotFit;
	//PrintResult recentPrintResult;
private:
	bool m_visible;
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
	ReportItemBreak(ReportItem *parent = nullptr);
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemFrame : public ReportItem
{
	Q_OBJECT
private:
	typedef ReportItem Super;
public:
	Q_CLASSINFO("DefaultProperty", "items")
	Q_PROPERTY(QQmlListProperty<qf::qmlwidgets::reports::ReportItem> items READ items)
	Q_ENUMS(HAlignment)
	Q_ENUMS(VAlignment)
	//Q_PROPERTY(qreal x1 READ x1 WRITE setX1 NOTIFY x1Changed)
	//Q_PROPERTY(qreal x2 READ x2 WRITE setX2 NOTIFY x2Changed)
	//Q_PROPERTY(qreal y1 READ y1 WRITE setY1 NOTIFY y1Changed)
	//Q_PROPERTY(qreal y2 READ y2 WRITE setY2 NOTIFY y2Changed)
	Q_PROPERTY(qreal hinset READ hinset WRITE setHinset NOTIFY hinsetChanged)
	Q_PROPERTY(qreal vinset READ vinset WRITE setVinset NOTIFY vinsetChanged)
	//Q_PROPERTY(qreal inset READ inset WRITE setInset NOTIFY insetChanged)
	Q_CLASSINFO("property.width.doc",
				"text jsou procenta napr 20% nebo jen % nebo nic nebo rozmer v mm\n"
				"% znamena rozdel zbytek na stejne casti\n"
				"n% znamena dej frame n% ze zbytku\n"
				"ve vertikalnim layoutu nejde kombinovat % a nezadany rozmery (atribut chybi)\n"
				"v takovem pripade se chybici atributy prevedou na '%'\n"
				"jedinou vyjjimkou je pripad, kdy ma % pouze posledni dite (ne treba '20%'', ale pouze '%'')\n"
				"v takovem pripade dostane child frame cely nepopsany zbytek rodice.")
	Q_PROPERTY(QVariant width READ width WRITE setWidth NOTIFY widthChanged)
	Q_PROPERTY(QVariant height READ height WRITE setHeight NOTIFY heightChanged)
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
	Q_PROPERTY(HAlignment halign READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
	Q_PROPERTY(VAlignment valign READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)
	Q_PROPERTY(QString columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(double columnsGap READ columnsGap WRITE setColumnsGap NOTIFY columnsGapChanged)
    Q_PROPERTY(qf::qmlwidgets::reports::style::Pen* border READ border WRITE setBorder NOTIFY borderChanged)
	Q_PROPERTY(qf::qmlwidgets::reports::style::Brush* fill READ fill WRITE setFill NOTIFY fillChanged)
	Q_CLASSINFO("property.textStyle.doc",
				"Set text style for this frame and all the children recursively"
				)
    Q_PROPERTY(qf::qmlwidgets::reports::style::Text* textStyle READ textStyle WRITE setTextStyle NOTIFY textStyleChanged)
public:
	enum HAlignment { AlignLeft = Qt::AlignLeft,
					  AlignRight = Qt::AlignRight,
					  AlignHCenter = Qt::AlignHCenter,
					  AlignJustify = Qt::AlignJustify };
	enum VAlignment { AlignTop = Qt::AlignTop,
					  AlignBottom = Qt::AlignBottom,
					  AlignVCenter = Qt::AlignVCenter };
	//QF_PROPERTY_IMPL(qreal, x, X, 1)
	//QF_PROPERTY_IMPL(qreal, y, Y, 1)
	//QF_PROPERTY_IMPL(qreal, x, X, 2)
	//QF_PROPERTY_IMPL(qreal, y, Y, 2)
	QF_PROPERTY_IMPL(qreal, h, H, inset)
	QF_PROPERTY_IMPL(qreal, v, V, inset)
	QF_PROPERTY_IMPL(QVariant, w, W, idth)
	QF_PROPERTY_IMPL(QVariant, h, H, eight)
	QF_PROPERTY_IMPL2(Layout, l, L, ayout, LayoutVertical)
	QF_PROPERTY_BOOL_IMPL(e, E, xpandChildrenFrames)
	QF_PROPERTY_IMPL2(HAlignment, h, H, orizontalAlignment, AlignLeft)
	QF_PROPERTY_IMPL2(VAlignment, v, V, erticalAlignment, AlignTop)
	QF_PROPERTY_IMPL2(QString, c, C, olumns, QStringLiteral("%"))
	QF_PROPERTY_IMPL2(double, c, C, olumnsGap, 3)
    QF_PROPERTY_OBJECT_IMPL(style::Pen*, b, B, order)
	QF_PROPERTY_OBJECT_IMPL(style::Brush*, f, F, ill)
    QF_PROPERTY_OBJECT_IMPL(style::Text*, t, T, extStyle)
public:
	ReportItemFrame(ReportItem *parent = nullptr);
	~ReportItemFrame() Q_DECL_OVERRIDE;
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

	//void setupMetaPaintItem(ReportItemMetaPaint *mpi) Q_DECL_OVERRIDE;

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
	ReportItemDetail(ReportItem *parent = nullptr);
	virtual ~ReportItemDetail() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBand : public ReportItemFrame
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
	//TODO: introduce dataModel property as ReportBandModel : public QObject ancestor implementing TreeTable interface
	//      Then data property can be removed
	Q_PROPERTY(qf::core::utils::TreeTable data READ data WRITE setData NOTIFY dataChanged)
	Q_PROPERTY(bool headerOnBreak READ isHeaderOnBreak WRITE setHeaderOnBreak NOTIFY headerOnBreakChanged)
	/*
	Q_PROPERTY(ReportItemFrame* header READ header WRITE setHeader NOTIFY headerChanged)
	Q_PROPERTY(ReportItemDetail* detail READ detail WRITE setDetail NOTIFY detailChanged)
	Q_PROPERTY(ReportItemFrame* footer READ footer WRITE setFooter NOTIFY footerChanged)
	*/
private:
	typedef ReportItemFrame Super;
public:
	ReportItemBand(ReportItem *parent = nullptr);
	~ReportItemBand() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QString, d, D, ataSource)
	QF_PROPERTY_IMPL(qf::core::utils::TreeTable, d, D, ata)
	QF_PROPERTY_BOOL_IMPL(h, H, eaderOnBreak)
	/*
	QF_PROPERTY_OBJECT_IMPL(ReportItemFrame*, h, H, eader)
	QF_PROPERTY_OBJECT_IMPL(ReportItemDetail*, d, D, etail)
	QF_PROPERTY_OBJECT_IMPL(ReportItemFrame*, f, F, ooter)
	*/
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	virtual  ReportItemBand* toBand()  {return this;}
	ReportItemDetail* detail();

	virtual qf::core::utils::TreeTable dataTable();
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
protected:
	qf::core::utils::TreeTable f_dataTable;
	bool dataTableLoaded;
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemReport : public ReportItemBand
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::reports::style::Sheet* styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)
	Q_PROPERTY(int debugLevel READ debugLevel WRITE setDebugLevel)
private:
	typedef ReportItemBand Super;
public:
	ReportItemReport(ReportItem *parent = nullptr);
	~ReportItemReport() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_OBJECT_IMPL(style::Sheet*, s, S, tyleSheet)
	QF_PROPERTY_IMPL2(int, d, D, ebugLevel, 0)
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	ReportProcessor* reportProcessor() {return m_reportProcessor;}
	void setReportProcessor(ReportProcessor *p) {m_reportProcessor = p;}
private:
	ReportProcessor *m_reportProcessor;
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
	ReportItemBody(ReportItem *parent = nullptr)
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

	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(bool omitEmptyText READ isOmitEmptyText WRITE setOmitEmptyText NOTIFY omitEmptyTextChanged)
	Q_PROPERTY(QString sqlId READ sqlId WRITE setSqlId NOTIFY sqlIdChanged)
	Q_PROPERTY(HAlignment textHAlign READ textHAlign WRITE setTextHAlign NOTIFY textHAlignChanged)
	Q_PROPERTY(VAlignment textVAlign READ textVAlign WRITE setTextVAlign NOTIFY textVAlignChanged)
	Q_PROPERTY(bool textWrap READ isTextWrap WRITE setTextWrap NOTIFY textWrapChanged)
private:
	typedef ReportItemFrame Super;

	QF_PROPERTY_IMPL(QString, t, T, ext)
	QF_PROPERTY_BOOL_IMPL2(o, O, mitEmptyText, true)
	QF_PROPERTY_IMPL(QString, s, S, qlId)
	QF_PROPERTY_IMPL2(HAlignment, t, T, extHAlign, AlignLeft)
	QF_PROPERTY_IMPL2(VAlignment, t, T, extVAlign, AlignTop)
	QF_PROPERTY_BOOL_IMPL2(t, T, extWrap, true)
public:
	ReportItemPara(ReportItem *parent = nullptr);
	virtual ~ReportItemPara() {}
protected:
	/// tiskne se printed text od indexToPrint, pouziva se pouze v pripade, ze text pretece na dalsi stranku
	QString printedText;
	QTextLayout textLayout;
protected:
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
	QString paraText();
public:
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printHtml(HTMLElement &out);
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemImage : public ReportItemFrame
{
	Q_OBJECT

	Q_ENUMS(DataFormat)
	Q_ENUMS(DataEncoding)
	Q_ENUMS(DataCompression)
	Q_ENUMS(AspectRatio)

	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
	Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)
	Q_PROPERTY(DataFormat dataFormat READ dataFormat WRITE setDataFormat NOTIFY dataFormatChanged)
	Q_PROPERTY(DataEncoding dataEncoding READ dataEncoding WRITE setDataEncoding NOTIFY dataEncodingChanged)
	Q_PROPERTY(DataCompression dataCompression READ dataCompression WRITE setDataCompression NOTIFY dataCompressionChanged)
	Q_PROPERTY(AspectRatio aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
	Q_PROPERTY(bool suppressPrintout READ isSuppressPrintout WRITE setSuppressPrintout NOTIFY suppressPrintoutChanged)
private:
	typedef ReportItemFrame Super;
public:
	ReportItemImage(ReportItem *parent = nullptr);
public:
	enum DataFormat {
		FormatAuto,
		FormatSvg,
		FormatQPicture,
		FormatPng,
		FormatJpg
	};
	enum DataEncoding {
		EncodingRaw,
		EncodingBase64,
		EncodingHex
	};
	enum DataCompression {
		CompressionNone,
		CompressionQCompress
	};
	enum AspectRatio {
		AspectRatioIgnore = Qt::IgnoreAspectRatio,
		AspectRatioKeep = Qt::KeepAspectRatio,
		AspectRatioKeepExpanding = Qt::KeepAspectRatioByExpanding
	};
	QF_PROPERTY_IMPL(QString, d, D, ataSource)
	QF_PROPERTY_IMPL(QString, d, D, ata)
	QF_PROPERTY_IMPL2(DataFormat, d, D, ataFormat, FormatAuto)
	QF_PROPERTY_IMPL2(DataEncoding, d, D, ataEncoding, EncodingRaw)
	QF_PROPERTY_IMPL2(DataCompression, d, D, ataCompression, CompressionNone)
	QF_PROPERTY_IMPL2(AspectRatio, a, A, spectRatio, AspectRatioIgnore)
	QF_PROPERTY_BOOL_IMPL(s, S, uppressPrintout)
	private:
		void updateResolvedDataSource(const QString &data_source);
protected:
	//--virtual bool childrenSynced();
	//--virtual void syncChildren();
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
protected:
	QString m_resolvedDataSource;
	//--bool childrenSyncedFlag;
	//--QDomElement fakeLoadErrorPara;
	//--QDomDocument fakeLoadErrorParaDocument;
};

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

