//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMFRAME_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMFRAME_H

#include "reportitem.h"
#include "../../qmlwidgetsglobal.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

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
	ReportItemFrame* toFrame() Q_DECL_OVERRIDE {return this;}

	//void setupMetaPaintItem(ReportItemMetaPaint *mpi) Q_DECL_OVERRIDE;

	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
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

	//! Nastavi u sebe a u deti indexToPrint na nulu, aby se vytiskly na dalsi strance znovu.
	void resetIndexToPrintRecursively(bool including_para_texts) Q_DECL_OVERRIDE;
	//--const QList<double>& gridLayoutSizes() {return f_gridLayoutSizes;}
	//--void setGridLayoutSizes(const QList<double> &szs) {f_gridLayoutSizes = szs;}

	QString toString(int indent = 2, int indent_offset = 0) Q_DECL_OVERRIDE;
private:
	static void addItemFunction(QQmlListProperty<ReportItem> *list_property, ReportItem *item);
	static ReportItem* itemAtFunction(QQmlListProperty<ReportItem> *list_property, int index);
	static void removeAllItemsFunction(QQmlListProperty<ReportItem> *list_property);
	static int countItemsFunction(QQmlListProperty<ReportItem> *list_property);
protected:
	virtual int itemsToPrintCount() {return itemCount();}
	virtual ReportItem* itemToPrintAt(int ix) {return itemAt(ix);}
private:
	QQmlListProperty<ReportItem> items();
	int itemCount() const;
	ReportItem* itemAt(int index);
protected:
	//! children, kterym se ma zacit pri tisku
	int indexToPrint;
private:
	QList<ReportItem*> m_items;
	//--QList<double> f_gridLayoutSizes;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMFRAME_H
