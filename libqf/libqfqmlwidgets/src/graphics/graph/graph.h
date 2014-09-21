
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_GRAPHICS_GRAPH_H
#define QF_QMLWIDGETS_GRAPHICS_GRAPH_H

#include "../../qmlwidgetsglobal.h"
#include "../stylecache.h"
#include "../graphics.h"

#include <qf/core/utils/treetable.h>
#include <qf/core/exception.h>

#include <QSharedData>
#include <QPainter>

#include <cmath>
//class QPainter;

namespace qf {
namespace qmlwidgets {
namespace graphics {

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT Graph
{
	public:
		typedef StyleCache::Style TextStyle;
	public:
		class QFQMLWIDGETS_DECL_EXPORT Axis
		{
			public:
				enum ValuesFrom {TakeValues, TakeOrder};
				enum Direction {DirectionX, DirectionY};
			private:
				class SharedDummyHelper {};
				struct Data : public QSharedData {
					QString label;
					QString labelStyle;
					QString labelFormat;
					double min, max, tick;
					QString serieName;
					ValuesFrom valuesFrom;
					Direction direction;
					Rect boundingRect;
					bool hidden;
					bool gridLines;

					Data( ): min(HUGE_VAL), max(-HUGE_VAL), tick(0), valuesFrom(TakeValues), direction(DirectionY), hidden(false),gridLines(false) {}
				};
				QSharedDataPointer<Data> d;
				static const Axis& sharedNull();
				Axis(SharedDummyHelper); /// null row constructor
			public:
				bool isNull() const {return d == sharedNull().d;}

				QString serieName() const {return d->serieName;}
				void setSerieName(const QString &nm) {d->serieName = nm;}
				QString label() const {return d->label;}
				void setLabel(const QString &lbl) {d->label = lbl;}
				QString labelStyle() const {return d->labelStyle;}
				void setLabelStyle(const QString &lblst) {d->labelStyle = lblst;}
				QString labelFormat() const {return d->labelFormat;}
				void setLabelFormat(const QString &fmt) {d->labelFormat = fmt;}
				double min() const {return d->min;}
				void setMin(double dd) {d->min = dd;}
				double max() const {return d->max;}
				void setMax(double dd) {d->max = dd;}
				double tick() const {return d->tick;}
				void setTick(double dd) {d->tick = dd;}
				Direction direction() const {return d->direction;}
				void setDirection(Direction dd) {d->direction = dd;}
				ValuesFrom valuesFrom() const {return d->valuesFrom;}
				void setValuesFrom(ValuesFrom dd) {d->valuesFrom = dd;}
				Rect boundingRect() const {return d->boundingRect;}
				void setBoundingRect(const Rect &r) {d->boundingRect = r;}
				bool isHidden() const {return d->hidden;}
				void setHidden(bool b) {d->hidden = b;}
				bool isGridLines() const {return d->gridLines;}
				void setGridLines(bool b) {d->gridLines = b;}

				/// vraci mm souradnici value v grafu
				qreal value2pos(qreal value, const Rect &grid_rect);
				qreal tickSize(const Rect &grid_rect);

				QString formatTickLabel(const QVariant &label_value);
				
				QString toString() const {
					return QString("min: %2, max: %3, tick: %4").arg(min()).arg(max()).arg(tick());
				}
			public:
				Axis();
		};
		class QFQMLWIDGETS_DECL_EXPORT Legend
		{
			public:
				typedef QList<QColor> ColorList;
			private:
				class SharedDummyHelper {};
				struct Data : public QSharedData {
					//ColorList colors;
					//QStringList labels;
					Rect boundingRect;
				};
				QSharedDataPointer<Data> d;
				static const Legend& sharedNull();
				Legend(SharedDummyHelper); /// null row constructor
			public:
				bool isNull() const {return d == sharedNull().d;}

				//const ColorList colors() const {return d->colors;}
				//void setColors(const ColorList &clst) {d->colors = clst;}
				//const QStringList labels() const {return d->labels;}
				//void setLabels(const QStringList &lbls) {d->labels = lbls;}
				Rect boundingRect() const {return d->boundingRect;}
				void setBoundingRect(const Rect &r) {d->boundingRect = r;}
			public:
				Legend();
		};
		class QFQMLWIDGETS_DECL_EXPORT Serie
		{
			public:
				//typedef QList<QColor> ColorList;
			private:
				class SharedDummyHelper {};
				struct Data : public QSharedData {
					QString columnCaption;
					QColor color;
					QVariantList values;
					QList<int> sortedValuesIndexes;
				};
				QSharedDataPointer<Data> d;
				static const Serie& sharedNull();
				Serie(SharedDummyHelper); /// null row constructor
			public:
				bool isNull() const {return d == sharedNull().d;}

				const QVariantList& values() const {return d->values;}
				QVariantList& valuesRef() {return d->values;}
				void setValues(const QVariantList &vals) {d->values = vals;}
				const QList<int>& sortedValuesIndexes() const {return d->sortedValuesIndexes;}
				void setSortedValuesIndexes(const QList<int> &ixs) {d->sortedValuesIndexes = ixs;}
				QColor color() const {return d->color;}
				void setColor(const QColor &c) {d->color = c;}

				QString columnCaption() const {return d->columnCaption;}
				void setColumnCaption(const QString &s) {d->columnCaption = s;}

				bool isSorted() const {return !sortedValuesIndexes().isEmpty();}
			public:
				Serie();
		};
	protected:
		typedef QMap<QString, Serie> SeriesMap;
		typedef QMap<QString, Axis> AxesMap;
		typedef QMap<QString, Legend> LegendMap;
		struct Data
		{
			qf::core::utils::TreeTable data;
			QVariantMap definition;
			QPainter *painter;
			Rect boundingRect;/// obdelnik celeho grafu vcetne os [mm]
			Rect gridRect; /// obdelnik grafu bez os (pouce to, kam se vykresluji data) [mm]
			StyleCache styleCache;
			SeriesMap seriesMap;
			AxesMap axesMap;
			LegendMap legendMap;

			Data() {
				painter = NULL;
			}
		};
		Data _d;
		Data *d;
	protected:
		QPainter* painter() {
			if(!d->painter) QF_EXCEPTION("painter is NULL");
			return d->painter;
		}
		const Rect& boundingRect() const {return d->boundingRect;}
		const Rect& gridRect() const {return d->gridRect;}

		AxesMap& axesMapRef() {return d->axesMap;}
		const AxesMap& axesMap() const {return d->axesMap;}
		Axis axisForSerie(const QString &colname);

		const LegendMap& legendMap() const {return d->legendMap;}
		const SeriesMap& seriesMap() const {return d->seriesMap;}

		qreal x2device(qreal x) {return qf::qmlwidgets::graphics::x2device(x, painter()->device());}
		qreal y2device(qreal y) {return qf::qmlwidgets::graphics::y2device(y, painter()->device());}
		qreal device2x(qreal x) {return qf::qmlwidgets::graphics::device2x(x, painter()->device());}
		qreal device2y(qreal y) {return qf::qmlwidgets::graphics::device2y(y, painter()->device());}
		Point mm2device(const Point &p) {return qf::qmlwidgets::graphics::mm2device(p, painter()->device());}
		Rect mm2device(const Rect &r) {return qf::qmlwidgets::graphics::mm2device(r, painter()->device());}
		Point device2mm(const Point &p) {return qf::qmlwidgets::graphics::device2mm(p, painter()->device());}
		Rect device2mm(const Rect &r) {return qf::qmlwidgets::graphics::device2mm(r, painter()->device());}

		QString title() const;

		virtual void createSeries();
		virtual void createAxes();

		virtual void drawAxis(const QString &colname, const Rect &_bounding_rect, bool do_not_draw = false);
		/// vraci legend id
		virtual QString drawLegend(const QDomElement &el_legend, const Rect &_bounding_rect = Rect(), bool do_not_draw = false);
		// vraci legend id
		//virtual QString createLegend(const QDomElement &el_legend);

		virtual void drawTitle();
		virtual Rect drawLegends(bool do_not_draw);
		virtual void drawAxes();
		virtual void drawSeries() {}
		virtual void drawBox();
		virtual void drawGrid();

		static QStringList colorNamesPull;
		static QColor colorForIndex(int ix);
	public:
		void setDefinition(const QVariantMap &def) {d->definition = def;}
		const QVariantMap& definition() const {return d->definition;}
		void setData(const qf::core::utils::TreeTable _data) {d->data = _data;}
		const qf::core::utils::TreeTable& data() const {return d->data;}

		void setStyleCache(const StyleCache &cache) {d->styleCache = cache;}
		StyleCache& styleCacheRef() {return d->styleCache;}
		const StyleCache& styleCache() const {return d->styleCache;}

		virtual void draw(QPainter *painter, const QSizeF &size);
	public:
		static Graph* createGraph(const QDomElement el_def, const qf::core::utils::TreeTable &data = qf::core::utils::TreeTable());
	public:
		Graph();
		Graph(const QVariantMap def, const qf::core::utils::TreeTable &data = qf::core::utils::TreeTable());
		virtual ~Graph();
};

class QFQMLWIDGETS_DECL_EXPORT HistogramGraph : public Graph
{
private:
	typedef Graph Super;
	protected:
		virtual void drawSeries();
	public:
		HistogramGraph() : Graph() {}
		HistogramGraph(const QVariantMap &def, const qf::core::utils::TreeTable &data = qf::core::utils::TreeTable())
		 : Super(def, data) {}
};

class QFQMLWIDGETS_DECL_EXPORT PieGraph : public Graph
{
private:
	typedef Graph Super;
protected:
	virtual void drawBox() {}
	virtual void drawSeries();
	public:
		PieGraph() : Graph() {}
		PieGraph(const QVariantMap &def, const qf::core::utils::TreeTable &data = qf::core::utils::TreeTable())
		 : Super(def, data) {}
};

}}}

#endif // QF_QMLWIDGETS_GRAPHICS_GRAPH_H

