#include "reportitemframe.h"

#include "reportitemband.h"
#include "reportpainter.h"
#include "reportprocessor.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets::reports;

//==========================================================
//                      ReportItemFrame
//==========================================================
ReportItemFrame::ReportItemFrame(ReportItem *parent)
	: Super(parent)
{
	qfLogFuncFrame();
	//m_x1 = -1;
	//m_y1 = -1;
	//m_x2 = -1;
	//m_y2 = -1;
	//m_hInset = m_vInset = 0;
	m_layout = LayoutVertical;
	m_expandChildrenFrames = false;
	m_horizontalAlignment = AlignLeft;
	m_verticalAlignment = AlignTop;

	indexToPrint = 0;
	//qfDebug() << QF_FUNC_NAME << "*******************" << el.tagName() << el.attribute("id");
}

ReportItemFrame::~ReportItemFrame()
{
	qfLogFuncFrame();
}

void ReportItemFrame::componentComplete()
{
	updateDesignedRect();
}

void ReportItemFrame::updateDesignedRect()
{
	qfLogFuncFrame();
	// update designed rect
	/*--
	Point p;
	if(x1() >= 0) {
		designedRect.flags |= Rect::LeftFixed;
		p.rx() = x1();
	}
	if(y1() >= 0) {
		designedRect.flags |= Rect::TopFixed;
		p.ry() = y1();
	}
	designedRect.setTopLeft(p);
	if(x2() >= 0) {
		designedRect.flags |= Rect::RightFixed;
		p.rx() = x2();
	}
	if(y2() >= 0) {
		designedRect.flags |= Rect::BottomFixed;
		p.ry() = y2();
	}
	designedRect.setBottomRight(p);
	--*/
	//qfDebug() << "\t" << __LINE__ << "designedRect:" << designedRect.toString();
	//static const QString S_PERCENT = "%";
	{
		QVariant v = width();
		qreal d = 0;
		bool ok;
		if(v.type() == QVariant::String) {
			qfc::String s;
			s = v.toString().trimmed();
			{
				if(s.value(-1) == '%') {
					s = s.slice(0, -1).trimmed();
					designedRect.horizontalUnit = Rect::UnitPercent;
				}
				if(!s.isEmpty()) {
					d = s.toDouble(&ok);
					if(!ok)
						qfWarning() << "Cannot convert" << s << "to real number." << this;
				}
				/*--
				if(d > 0) {
					if(designedRect.flags & Rect::RightFixed) {
						qreal r = designedRect.right();
						designedRect.setWidth(d);
						designedRect.moveRight(r);
					}
					else
						designedRect.setWidth(d);
				}
				--*/
			}
		}
		else if(v.isValid()) {
			d = v.toReal(&ok);
			if(!ok)
				qfWarning() << "Cannot convert" << v.toString() << "to real number." << this;
		}
		designedRect.setWidth(d);
	}
	{
		QVariant v = height();
		qreal d = 0;
		bool ok;
		if(v.type() == QVariant::String) {
			qfc::String s;
			s = v.toString().trimmed();
			{
				if(s.value(-1) == '%') {
					s = s.slice(0, -1).trimmed();
					designedRect.verticalUnit = Rect::UnitPercent;
				}
				if(!s.isEmpty()) {
					d = s.toDouble(&ok);
					if(!ok)
						qfWarning() << "Cannot convert" << s << "to real number." << this;
				}
				/*--
				if(d > 0) {
					if(designedRect.flags & Rect::BottomFixed) {
						qreal b = designedRect.bottom();
						designedRect.setWidth(d);
						designedRect.moveBottom(b);
					}
					else
						designedRect.setHeight(d);
				}
				--*/
			}
		}
		else if(v.isValid()) {
			d = v.toReal(&ok);
			if(!ok)
				qfWarning() << "Cannot convert" << v.toString() << "to real number." << this;
		}
		designedRect.setHeight(d);
	}

	if(isExpandChildrenFrames()) {
		designedRect.flags |= Rect::ExpandChildrenFrames;
	}

	if(layout() == LayoutHorizontal)
		designedRect.flags |= Rect::LayoutHorizontalFlag;
	else
		designedRect.flags |= Rect::LayoutVerticalFlag;

	qfDebug() << "\tdesignedRect:" << designedRect.toString();
}

QString ReportItemFrame::toString(int indent, int indent_offset)
{
	QString ret = Super::toString(indent, indent_offset);
	for(int i=0; i<itemCount(); i++) {
		ret += '\n';
		ReportItem *it = itemAt(i);
		ret += it->toString(indent, indent_offset += indent);
	}
	return ret;
}

QQmlListProperty<ReportItem> ReportItemFrame::items()
{
	return QQmlListProperty<ReportItem>(this, 0,
										ReportItemFrame::addItemFunction,
										ReportItemFrame::countItemsFunction,
										ReportItemFrame::itemAtFunction,
										ReportItemFrame::removeAllItemsFunction
										);
}

void ReportItemFrame::addItem(ReportItem *item)
{
	if (item) {
		item->setParent(this);
		this->m_items << item;
		ReportItemFrame *frm = qobject_cast<ReportItemFrame*>(item);
		if(frm)
			frm->updateDesignedRect();
	}
}

void ReportItemFrame::addItemFunction(QQmlListProperty<ReportItem> *list_property, ReportItem *item)
{
	if (item) {
		ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
		item->setParent(that);
		that->m_items << item;
	}
}

ReportItem *ReportItemFrame::itemAtFunction(QQmlListProperty<ReportItem> *list_property, int index)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	return that->m_items.value(index);
}

void ReportItemFrame::removeAllItemsFunction(QQmlListProperty<ReportItem> *list_property)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	qDeleteAll(that->m_items);
	that->m_items.clear();
}

int ReportItemFrame::countItemsFunction(QQmlListProperty<ReportItem> *list_property)
{
	ReportItemFrame *that = static_cast<ReportItemFrame*>(list_property->object);
	return that->itemCount();
}

int ReportItemFrame::itemCount() const
{
	return m_items.count();
}

ReportItem *ReportItemFrame::itemAt(int index)
{
	return m_items[index];
}
/*
void ReportItemFrame::setupMetaPaintItem(ReportItemMetaPaint *mpit)
{
	Super::setupMetaPaintItem(mpit);
	style::Text *pts = effectiveTextStyle();
	if(pts) {
		style::CompiledTextStyle ts = pts->textStyle();
		mpit->setTextStyle(ts);
	}
}
*/
ReportItem::ChildSize ReportItemFrame::childSize(Layout parent_layout)
{
	if(parent_layout == LayoutHorizontal)
		return ChildSize(designedRect.width(), designedRect.horizontalUnit);
	return ChildSize(designedRect.height(), designedRect.verticalUnit);
}

ReportItem::PrintResult ReportItemFrame::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame();// << element.tagName() << "id:" << element.attribute("id") << "itemCount:" << itemsToPrintCount() << "indexToPrint:" << indexToPrint;
	qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	PrintResult res = PrintOk;
	//dirtySize = Size();
	Rect paint_area_rect = bounding_rect;
	//Size children_dirty_size;
	//Size children_bounding_size = bbr.size();

	if(layout() == LayoutHorizontal) {
		/// horizontalni layout musi mit procenta rozpocitany dopredu, protoze jinak by se mi nezalamovaly texty v tabulkach
		/// v horizontalnim layoutu break ignoruj
		QList<ChildSize> sizes;
		/// pri tisku horizontalniho layoutu se tiskne vzdy od zacatku
		indexToPrint = 0;
		/*--
		ReportItemFrame *parent_grid = NULL;
		if(isParentGridLayout()) {
			ReportItem *it = this->parent();
			while(it) {
				ReportItemFrame *frm_it = qobject_cast<ReportItemFrame*>(it);
				if(frm_it && frm_it->isParentGrid()) {
					parent_grid = frm_it;
					break;
				}
				it = it->parent();
			}
		}
		bool is_first_grid_child = (parent_grid && parent_grid->gridLayoutSizes().isEmpty());
		bool is_next_grid_child = (parent_grid && !is_first_grid_child);

		if(is_next_grid_child) {
			/// dalsi deti gridu si zkopiruji rozmery z prvniho ditete, ktere je jiz vytisknute
			QList<double> szs = parent_grid->gridLayoutSizes();
			for(int i=indexToPrint; i<childrenCount(); i++) {
				ReportItem *it = itemAt(i);
				ChildSize sz;
				if(i < szs.count()) {
					it->designedRect.horizontalUnit = Rect::UnitMM;
					it->designedRect.setWidth(szs[i]); /// nastav mu velikost z prvniho tisku
				}
				else {
					qfWarning() << "this should never happen with grid layout, grid layout rows have different cell numbers, falling to original size values.";
				}
			}
		}
		--*/
		{
			/// prvni dite gridu se tiskne jako vse ostatni
			/// nastav detem mm rozmer ve smeru layoutu
			for(int i=indexToPrint; i<itemsToPrintCount(); i++) {
				ReportItem *it = itemToPrintAt(i);
				sizes << it->childSize(layout());
			}
		}

		/// zbyva vypocitat jeste ortogonalni rozmer
		/// je to bud absolutni hodnota nebo % z bbr
		QList<ChildSize> orthogonal_sizes;
		for(int i=indexToPrint; i<itemsToPrintCount(); i++) {
			ReportItem *it = itemToPrintAt(i);
			Layout ol = orthogonalLayout();
			ChildSize sz = it->childSize(ol);
			if(sz.unit == Rect::UnitPercent) {
				sz.size = paint_area_rect.sizeInLayout(ol); /// udelej z nej rubber, roztahne se dodatecne
				//if(sz.size == 0) sz.size = bbr.sizeInLayout(ol);
				//else sz.size = sz.size / 100 * bbr.sizeInLayout(ol);
			}
			orthogonal_sizes << sz;
			//if(it->isBreak() && i > indexToPrint && layout() == LayoutVertical) break; /// v horizontalnim layoutu break ignoruj
		}

		// can_print_children_in_natural_order je true v pripade, ze procenta nejsou nebo 1. procenta jsou za poslednim rubber frame.
		//bool can_print_children_in_natural_order = first_percent_ix < 0 || last_rubber_ix < 0 || last_rubber_ix < first_percent_ix;

		{
			/// v horizontalnim layoutu vytiskni nejdriv fixed itemy, pak rubber, ze zbytku rozpocitej % a vytiskni je taky
			/// vytiskly itemy pak rozsoupej do spravnyho poradi

			QMap<int, int> poradi_tisku; /// layout_ix->print_ix
			qreal sum_mm = 0;
			bool has_percent = false;
			/// vytiskni rubber a fixed
			//if(parent_grid) qfWarning() << (is_first_grid_child? "first child": is_next_grid_child? "next child": "nevim");
			for(int i=0; i<itemsToPrintCount(); i++) {
				ReportItem *it = itemToPrintAt(i);
				ChildSize sz = sizes.value(i);
				//qfInfo() << "child:" << i << "size:" << sz.size << "unit:" << Rect::unitToString(sz.unit);
				if(sz.unit == Rect::UnitMM) {
					Rect ch_bbr = paint_area_rect;
					ch_bbr.setLeft(paint_area_rect.left() + sum_mm);
					if(sz.size > 0) ch_bbr.setWidth(sz.size);
					else ch_bbr.setWidth(paint_area_rect.width() - sum_mm);
					if(orthogonal_sizes[i].size > 0) {
						ch_bbr.setSizeInLayout(orthogonal_sizes[i].size, orthogonalLayout());
					}
					//qfInfo() << "\t tisknu fixed:" << it->designedRect.toString();
					int prev_children_cnt = out->childrenCount();
					PrintResult ch_res = it->printMetaPaint(out, ch_bbr);
					if(out->children().count() > prev_children_cnt) {
						//qfInfo() << "rubber fixed:" << i << "->" << prev_children_cnt;
						poradi_tisku[i] = prev_children_cnt;
						double width = out->lastChild()->renderedRect.width();
						sum_mm += width;
						//if(parent_grid) qfInfo() << "\t renderedRect:" << out->lastChild()->renderedRect.toString();
						//}
						//qfInfo() << "\t sum_mm:" << sum_mm;
						if(ch_res.value != PrintOk) {
							/// para se muze vytisknout a pritom bejt not fit, pokud pretece
							res = ch_res;
						}
					}
					else {
						if(ch_res.value == PrintOk) {
							/// jediny, kdo se nemusi vytisknout je band
							if(it->isVisible()) {
								qfWarning() << "jak to, ze se dite nevytisklo v horizontalnim layoutu?" << it;
							}
						}
						else {
							//qfInfo() << "\t NOT OK";
							res = ch_res;
							break;
						}
					}
				}
				else {
					has_percent = true;
				}
			}
			qreal rest_mm = paint_area_rect.width() - sum_mm;

			if(res.value == PrintOk) {
				if(has_percent) {
					/// rozpocitej procenta
					qreal sum_percent = 0;
					int cnt_0_percent = 0;
					for(int i=0; i<itemsToPrintCount(); i++) {
						ReportItem *it = itemToPrintAt(i);
						ChildSize sz = it->childSize(layout());
						if(sz.unit == Rect::UnitPercent) {
							if(sz.size == 0) cnt_0_percent++;
							else sum_percent += sz.size;
						}
					}
					if(rest_mm <= 0) {
						qfWarning() << "Percent exist but rest_mm is" << rest_mm << ". Ignoring rest of frames";
					}
					else {
						/// vytiskni procenta
						qreal percent_0 = 0;
						if(cnt_0_percent > 0) percent_0 = (100 - sum_percent) / cnt_0_percent;
						for(int i=0; i<itemsToPrintCount(); i++) {
							ReportItem *it = itemToPrintAt(i);
							ChildSize sz = it->childSize(layout());
							if(sz.unit == Rect::UnitPercent) {
								qreal d;
								if(sz.size == 0) d = rest_mm * percent_0 / 100;
								else d = rest_mm * sz.size / 100;
								//qfInfo() << d;
								Rect ch_bbr = paint_area_rect;
								ch_bbr.setWidth(d);
								if(orthogonal_sizes[i].size > 0) {
									ch_bbr.setSizeInLayout(orthogonal_sizes[i].size, orthogonalLayout());
								}
								//qfInfo() << it << "tisknu percent" << it->designedRect.toString();
								//qfInfo() << "chbr" << ch_bbr.toString();
								int prev_children_cnt = out->childrenCount();
								PrintResult ch_res = it->printMetaPaint(out, ch_bbr);
								if(out->children().count() > prev_children_cnt) {
									//qfInfo() << "percent:" << i << "->" << prev_children_cnt;
									poradi_tisku[i] = prev_children_cnt;
									if(ch_res.value != PrintOk) {
										/// para se muze vytisknout a pritom bejt not fit, pokud pretece
										res = ch_res;
									}
								}
								else {
									if(ch_res.value == PrintOk) {
										/// jediny, kdo se nemusi vytisknout je band
										if(it->isVisible()) {
											qfWarning() << "jak to, ze se dite nevytisklo v horizontalnim layoutu?" << it;
										}
									}
									else {
										res = ch_res;
										break;
									}
								}
							}
						}
					}
					/// posprehazej vytisknuty deti
					//qfInfo() << "\t poradi tisku cnt:<<" << poradi_tisku.count() << out->childrenCount();
					if(poradi_tisku.count() == out->children().count()) {
						int children_count = out->children().count();
						//qfInfo() << "children cnt:" << children_count;
						//QF_ASSERT(poradi_tisku.count() == out->children().count(), "nevytiskly se vsechny deti v horizontalnim layoutu");
						QVector<qf::core::utils::TreeItemBase*> old_children(poradi_tisku.count());
						/// zkopiruj ukazatele na deti
						for(int i=0; i<children_count; i++)
							old_children[i] = out->children()[i];
						/// dej je do spravnyho poradi
						/// v mape nemusi byt rada klicu souvisla (kdyz se nejake dite nevytiskne)
						QMapIterator<int, int> iter(poradi_tisku);
						int new_print_ix = 0;
						while(iter.hasNext()) {
							iter.next();
							int old_print_ix = iter.value();
							if(0 <= new_print_ix && new_print_ix < children_count) {
								//qfInfo() << old_print_ix << "->" << new_print_ix;
								if(new_print_ix != old_print_ix) out->childrenRef()[new_print_ix] = old_children[old_print_ix];
								new_print_ix++;
							}
							else qfWarning() << QF_FUNC_NAME << "poradi:" << old_print_ix << "new_ix:" << new_print_ix << "out of range:" << children_count;
						}

						/// nastav detem spravne offsety
						qreal offset_x = 0;
						for(int i=0; i<poradi_tisku.count(); i++) {
							//qfInfo() << "\t poradi tisku <<" << i << "offset:" << offset_x;
							ReportItemMetaPaint *it = out->child(i);
							/// tady je to potreba posunout vcetne deti :(
							double shift_x = paint_area_rect.left() + offset_x - it->renderedRect.left();
							//if(parent_grid) qfInfo() << i << "offset_x:" << offset_x << "bbr left:" << bbr.left() << "chbbr left:" << ch_bbr.left();
							if(qFloatDistance(shift_x, 0) > 200)
								it->shift(Point(shift_x, 0));
							offset_x += it->renderedRect.width();
						}
					}
					else {
						qfWarning() << this << "nesedi poradi pocty tisku" << poradi_tisku.count() << out->children().count();
					}
				}
				/*--
				if(is_first_grid_child) {
					QList<double> szs;
					for(int i=0; i<out->children().count(); i++) {
						//qfInfo() << "\t poradi tisku <<" << i << "offset:" << offset_x;
						ReportItemMetaPaint *it = out->child(i);
						szs << it->renderedRect.width();
					}
					parent_grid->setGridLayoutSizes(szs);
				}
				--*/
			}
			if(res.value != PrintOk) {
				/// detail by mel mit, pokud se ma zalamovat, vzdy vertikalni layout, jinak tato funkce zpusobi, ze se po zalomeni vsechny dcerine bandy budou tisknout cele znova
				/// zakomentoval jsem to a zda se, ze to zatim nicemu nevadi
				//resetIndexToPrintRecursively(!ReportItem::IncludingParaTexts);
			}
		}
	}
	else {
		/// vertical layout
		/// print % like a rubber dimension and if print result is PrintOk resize rendered rect of printed metapaint item
		/// break funguje tak, ze pri 1., 3., 5. atd. tisku vraci PrintNotFit a pri sudych PrintOk
		/// prvni break na strance znamena, ze jsem tu uz po zalomeni, takze se tiskne to za break.
		//if(it->isBreak() && i > indexToPrint && layout == LayoutVertical) break;
		int index_to_print_0 = indexToPrint;
		for(; indexToPrint<itemsToPrintCount(); indexToPrint++) {
			ReportItem *it = itemToPrintAt(indexToPrint);
			Rect children_paint_area_rect = paint_area_rect;
			qfDebug() << "\tch_bbr v1:" << children_paint_area_rect.toString();

			{
				/// find child paint area size in layout direction
				qreal d = children_paint_area_rect.sizeInLayout(layout());
				ChildSize sz = it->childSize(layout());
				//qfInfo() << it << "chbrd:" << ch_bbr.toString() << "d:" << d;// << "size in ly:" << sz.fillLayoutRatio();
				if(sz.fillLayoutRatio() >= 0) {
				}
				else if(sz.unit == Rect::UnitMM) {
					if(sz.size > 0)
						d = sz.size;
				}
				else {
					ReportItemFrame *frit = qobject_cast<ReportItemFrame*>(it);
					if(frit)
						qfWarning() << "This should never happen" << it;
				}
				//qfInfo() << "\t ch_bbr.sizeInLayout(layout():" << ch_bbr.sizeInLayout(layout()) << "d:" << d;
				d = qMin(children_paint_area_rect.sizeInLayout(layout()), d);
				children_paint_area_rect.setSizeInLayout(d, layout());
				//qfInfo() << "\t ch_bbr:" << ch_bbr.toString();
			}
			{
				/// orthogonal size
				Layout ol = orthogonalLayout();
				ChildSize o_sz = it->childSize(ol);
				if(o_sz.unit == Rect::UnitPercent) {
					if(o_sz.size == 0)
						o_sz.size = paint_area_rect.sizeInLayout(ol);
					else
						o_sz.size = o_sz.size / 100 * paint_area_rect.sizeInLayout(ol);
				}
				//it->metaPaintOrthogonalLayoutLength = sz.size;
				qfDebug() << "\tsetting orthogonal length:" << o_sz.size;
				//if(it->isBreak() && i > indexToPrint && layout == LayoutVertical) break; /// v horizontalnim layoutu break ignoruj
				if(o_sz.size > 0) {
					children_paint_area_rect.setSizeInLayout(o_sz.size, orthogonalLayout());
				}
			}
			qfDebug() << "\tch_bbr v2:" << children_paint_area_rect.toString();
			int prev_children_cnt = out->childrenCount();
			PrintResult ch_res = it->printMetaPaint(out, children_paint_area_rect);
			if(ch_res.value == PrintOk) {
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
				//dirtyRect = dirtyRect.unite(it->dirtyRect);
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
				/// muze se stat, ze se dite nevytiskne, napriklad band nema zadna data
				//QF_ASSERT(out->children().count() > 0, "jak to, ze se dite nevytisklo?");
				if(out->children().count() > prev_children_cnt) {
					ReportItemMetaPaint *mpi = out->lastChild();
					//if(fill_vertical_layout_ratio >= 0) mpi->setFillVLayoutRatio(fill_vertical_layout_ratio);
					if(mpi) {
						const Rect &r = mpi->renderedRect;
						//qfInfo() << mpi << mpi->reportItem()->element.tagName() << (r.flags & Rect::BackgroundItem) << "\tr:" << r.toString() << "ch_res:" << ch_res.toString();
						//if((r.flags & Rect::BackgroundItem)) qfWarning() << "BackgroundItem";
						//--if(!(r.flags & Rect::BackgroundItem))
						/// cut rendered area
						paint_area_rect.cutSizeInLayout(r, layout());
						if(ch_res.flags & FlagPrintAgain) {
							indexToPrint--; /// vytiskni ho znovu
						}
					}
				}
				//bbr.cutSizeInLayout(it->dirtyRect, layout);
				//qfDebug() << "\t" << __LINE__ << "children_dirty_size:" << children_dirty_size.toString();
			}
			else {
				/// pokud je vertikalni layout, a dite se nevejde vrat PrintNotFit
				if(layout() == LayoutHorizontal) {
					/// v horizontalnim, zadne pretikani neni
					/// vytiskni to znovu s doteklymi texty
					qfError() << "Oops??? this should never happen, I'm already in the vertical layout'";
					resetIndexToPrintRecursively(!ReportItem::IncludingParaTexts);
				}
				res = ch_res;
				break;
			}
			if(it->isBreak() && indexToPrint > index_to_print_0)
				break;
		}
	}
	//res = checkPrintResult(res);
	qfDebug() << "\t<<< CHILDREN return:" << res.toString();
	return res;
}

ReportItem::PrintResult ReportItemFrame::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	qfDebug() << "\tbounding_rect:" << bounding_rect.toString();
	qfDebug() << "\tdesignedRect:" << designedRect.toString();// << "isLeftTopFloating:" << isLeftTopFloating() << "isRightBottomFloating:" << isRightBottomFloating();
	qfDebug() << "\tlayout:" << ((layout() == LayoutHorizontal)? "horizontal": "vertical") << ", is rubber:" << isRubber(layout());
	PrintResult res = PrintOk;
	if(!isVisible())
		return res;
	Rect frame_content_br = bounding_rect;
	qfDebug() << "\tbbr 0:" << frame_content_br.toString();
	if(designedRect.horizontalUnit == Rect::UnitMM && designedRect.width() - Epsilon > bounding_rect.width()) {
		qfDebug() << "\t<<<< FRAME NOT FIT WIDTH";
		return checkPrintResult(PrintNotFit);
	}
	if(designedRect.verticalUnit == Rect::UnitMM && designedRect.height() - Epsilon > bounding_rect.height()) {
		qfDebug() << "\t<<<< FRAME NOT FIT HEIGHT";
		return checkPrintResult(PrintNotFit);
	}
	frame_content_br.adjust(hinset(), vinset(), -hinset(), -vinset());

	QList<double> column_sizes;
	double columns_gap = 0;
	//int current_column_index;
	if(column_sizes.isEmpty()) {
		qf::core::String s = columns();
		QStringList sl = s.splitAndTrim(',');
		columns_gap = columnsGap();
		double ly_size = frame_content_br.width() - (columns_gap * (sl.count() - 1));
		column_sizes = qf::qmlwidgets::graphics::makeLayoutSizes(sl, ly_size);
		//current_column_index = 0;
	}
	ReportItemMetaPaintFrame *mp = new ReportItemMetaPaintFrame(out, this);
	QF_ASSERT_EX(mp != nullptr, "Meta paint item for item " + QString(this->metaObject()->className()) + " not created.");
	mp->setInset(hinset(), vinset());
	mp->setLayout((qf::qmlwidgets::graphics::Layout)layout());
	mp->setAlignment(horizontalAlignment(), verticalAlignment());
	Rect column_br_helper = frame_content_br;
	for(int current_column_index=0; current_column_index<column_sizes.count(); current_column_index++) {
		Rect column_br = column_br_helper;
		column_br.setWidth(column_sizes.value(current_column_index));
		qfDebug() << "\tcolumn bounding rect:" << column_br.toString();

		res = printMetaPaintChildren(mp, column_br);
		//qfDebug() << "\tbbr_init:" << bbr_init.toString();

		if(res.value == PrintNotFit) {
			//qfInfo().color(QFLog::Yellow) << element.tagName() << "keep all:" << keepAll << "column" << current_column_index << "of" << column_sizes.count();
			//qfInfo().color(QFLog::Yellow) << "column_br:" << column_br.toString() << "frame_content_br:" << frame_content_br.toString();
			/// pokud je result neverfit, nech ho tam, at aspon vidime, co se nikdy nevejde
			if(isKeepAll() && !(res.flags & FlagPrintNeverFit)) {
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				//qfInfo() << "keepAll && !(res.flags & FlagPrintNeverFit)";
				QF_SAFE_DELETE(mp);
				//qfInfo().color(QFLog::Green) << "return" << current_column_index << "of" << column_sizes.count();
				return checkPrintResult(res);
			}
		}
		column_br_helper = column_br;
		column_br_helper.moveLeft(column_br.right() + columns_gap);
	}

	/// tak kolik jsem toho pokreslil?
	Rect dirty_rect;//, rendered_rect = designedRect;
	dirty_rect.flags = designedRect.flags;
	{
		/// musim to proste secist
		for(int i=0; i<mp->childrenCount(); i++) {
			ReportItemMetaPaint *it = mp->child(i);
			if(dirty_rect.isNull())
				dirty_rect = it->renderedRect;
			else
				dirty_rect = dirty_rect.united(it->renderedRect);
		}
		qfDebug() << "\trubber dirty_rect:" << dirty_rect.toString();
	}
	//qfWarning() << this << "\tdirty_rect 1:" << dirty_rect.toString();
	/// pokud je v nekterem smeru definovany, je jedno, kolik se toho potisklo a nastav ten rozmer
	if(designedRect.horizontalUnit == Rect::UnitPercent)
		dirty_rect.setWidth(frame_content_br.width()); /// horizontalni rozmer musi ctit procenta
	else if(designedRect.horizontalUnit == Rect::UnitMM && designedRect.width() > Epsilon)
		dirty_rect.setWidth(designedRect.width() - 2*hinset());
	if(designedRect.verticalUnit == Rect::UnitMM && designedRect.height() > Epsilon)
		dirty_rect.setHeight(designedRect.height() - 2*vinset());
	//qfWarning() << "\tdirty_rect 3:" << dirty_rect.toString();
	/// pri rendrovani se muze stat, ze dirtyRect nezacina na bbr, to ale alignment zase spravi
	dirty_rect.moveTopLeft(frame_content_br.topLeft());
	//qfWarning() << "\tdirty_rect:" << dirty_rect.toString();
	//qfDebug() << "\tlayout:" << ((layout == LayoutHorizontal)? "horizontal": "vertical");
	//qfDebug() << "\tortho layout:" << ((orthogonalLayout() == LayoutHorizontal)? "horizontal": "vertical");
	//qfDebug() << "\trenderedRect:" << r.toString();

	/// alignment
	//qfDebug() << "\tALIGN:" << QString::number((int)alignment, 16);
	//alignChildren(mp, dirty_rect);
	//if(0)
	dirty_rect.adjust(-hinset(), -vinset(), hinset(), vinset());
	mp->renderedRect = dirty_rect;
	/// aby sly expandovat deti, musi mit parent spravne renderedRect
	//qfInfo() << this << "rendered rect2:" << mp->renderedRect.toString();
	if(res.value == PrintOk || (res.value == PrintNotFit && (res.flags & FlagPrintBreak))) {
		//bool children_aligned = false;
		/// pokud se vytiskl layout, jehoz vyska nebyla zadana jako % a ma dite s %, roztahni dite a pripadne i jeho deti
		//qfInfo() << childSize(LayoutVertical).fillLayoutRatio();
		if(childSize(LayoutVertical).fillLayoutRatio() < 0) {
			mp->expandChildVerticalSpringFrames();
		}
		//qfInfo() << "\t rendered rect2:" << mp->renderedRect.toString();
		mp->alignChildren();
		mp->renderedRect.flags = designedRect.flags;
		/// mohl bych to udelat tak, ze bych vsem detem dal %, ale je to moc klikani v repeditu
		if(designedRect.flags & ReportItem::Rect::ExpandChildrenFrames) {
			mp->expandChildrenFramesRecursively();
		}
	}
	//dirtyRect = r;//.adjusted(-hinset, -vinset, hinset, vinset);;
	qfDebug() << "\trenderedRect:" << mp->renderedRect.toString();
	res = checkPrintResult(res);
	//qfDebug().color(QFLog::Cyan) << "\t<<<< FRAME return:" << res.toString() << element.tagName() << "id:" << element.attribute("id");
	return res;
}
/*
void ReportItemFrame::alignChildren(ReportItemMetaPaintFrame *mp,  const ReportItem::Rect & dirty_rect)
{
	if(!dirty_rect.isNull()) {
		if(alignment & ~(Qt::AlignLeft | Qt::AlignTop)) {
			Point offset;
			/// ve smeru layoutu posun cely blok
			{
				Rect r1;
				/// vypocitej velikost potisknuteho bloku
				for(int i=0; i<mp->childrenCount(); i++) {
					ReportItemMetaPaint *it = mp->itemAt(i);
					r1 = r1.united(it->renderedRect);
				}
				qreal al = 0, d;
				if(layout == LayoutHorizontal) {
					if(alignment & Qt::AlignHCenter) al = 0.5;
					else if(alignment & Qt::AlignRight) al = 1;
					d = dirty_rect.width() - r1.width();
					if(al > 0 && d > 0)  {
						offset.rx() = d * al - (r1.left() - dirty_rect.left());
					}
				}
				else if(layout == LayoutVertical) {
					if(alignment & Qt::AlignVCenter) al = 0.5;
					else if(alignment & Qt::AlignBottom) al = 1;
					d = dirty_rect.height() - r1.height();
					if(al > 0 && d > 0)  {
						offset.ry() = d * al - (r1.top() - dirty_rect.top());
					}
				}
			}

			/// v orthogonalnim smeru kazdy item
			for(int i=0; i<mp->childrenCount(); i++) {
				ReportItemMetaPaint *it = mp->itemAt(i);
				const Rect &r1 = it->renderedRect;
				qfDebug() << "\t\titem renderedRect:" << r1.toString();
				qreal al = 0, d;

				if(orthogonalLayout() == LayoutHorizontal) {
					offset.rx() = 0;
					if(alignment & Qt::AlignHCenter) al = 0.5;
					else if(alignment & Qt::AlignRight) al = 1;
					d = dirty_rect.width() - r1.width();
					if(al > 0 && d > 0)  {
						qfDebug() << "\t\thorizontal alignment:" << al;
						offset.rx() = d * al - (r1.left() - dirty_rect.left());
					}
				}
				else if(orthogonalLayout() == LayoutVertical) {
					offset.ry() = 0;
					al = 0;
					if(alignment & Qt::AlignVCenter) al = 0.5;
					else if(alignment & Qt::AlignBottom) al = 1;
					d = dirty_rect.height() - r1.height();
					if(al > 0 && d > 0)  {
						qfDebug() << "\t\tvertical alignment:" << al;
						offset.ry() = d * al - (r1.top() - dirty_rect.top());
					}
				}
				qfDebug() << "\t\talign offset:" << offset.toString();
				if(!offset.isNull()) it->shift(offset);
			}
		}
	}
}
*/
/*
ReportItemFrame::Layout ReportItemFrame::parentLayout() const
{
	ReportItemFrame *frm = parentFrame();
	if(!frm) return LayoutInvalid;
	return frm->layout;
}
*/
void ReportItemFrame::resetIndexToPrintRecursively(bool including_para_texts)
{
	//qfInfo() << "resetIndexToPrintRecursively()";
	indexToPrint = 0;
	for(int i=0; i<itemsToPrintCount(); i++) {
		ReportItem *it = itemToPrintAt(i);
		it->resetIndexToPrintRecursively(including_para_texts);
	}
	/*
	foreach(QObject *o, children()) {
		ReportItem *it = qobject_cast<ReportItem*>(o);
		it->resetIndexToPrintRecursively();
	}
	*/
}

