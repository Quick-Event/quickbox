#ifndef RUNSTABLEITEMDELEGATE_H
#define RUNSTABLEITEMDELEGATE_H

#include <quickevent/og/itemdelegate.h>

#include <qf/core/utils.h>

class RunsTableItemDelegate : public quickevent::og::ItemDelegate
{
	Q_OBJECT
private:
	typedef quickevent::og::ItemDelegate Super;
public:
	RunsTableItemDelegate(qf::qmlwidgets::TableView * parent = nullptr);

	QF_PROPERTY_BOOL_IMPL(s, S, tartTimeHighlightVisible)

	void setHighlightedClassId(int class_id);

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
	int m_highlightedClassId = 0;
	int m_classStart;
	int m_classInterval;
};

#endif // RUNSTABLEITEMDELEGATE_H
