#ifndef RUNSTABLEITEMDELEGATE_H
#define RUNSTABLEITEMDELEGATE_H

#include <quickevent/gui/og/itemdelegate.h>

#include <qf/core/utils.h>

class RunsTableItemDelegate : public quickevent::gui::og::ItemDelegate
{
	Q_OBJECT
private:
	typedef quickevent::gui::og::ItemDelegate Super;
public:
	RunsTableItemDelegate(qf::qmlwidgets::TableView * parent = nullptr);

	QF_PROPERTY_BOOL_IMPL2(s, S, tartTimeHighlightVisible, false)

	void setHighlightedClassId(int class_id, int stage_id);
	void reloadHighlightedClassId();

	//void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &ix) const Q_DECL_OVERRIDE;
protected:
	void paintBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
	int m_stageId = 0;
	int m_highlightedClassId = 0;
	int m_classStartFirst = 0;
	int m_classStartLast = 0;
	int m_classInterval = 0;
};

#endif // RUNSTABLEITEMDELEGATE_H
