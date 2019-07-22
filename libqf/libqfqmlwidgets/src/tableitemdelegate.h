#ifndef QF_QMLWIDGETS_TABLEITEMDELEGATE_H
#define QF_QMLWIDGETS_TABLEITEMDELEGATE_H

#include "qmlwidgetsglobal.h"

#include <QStyledItemDelegate>

namespace qf {
namespace qmlwidgets {

class TableView;

class QFQMLWIDGETS_DECL_EXPORT TableItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
private:
	typedef QStyledItemDelegate Super;
public:
	explicit TableItemDelegate(TableView *parent = nullptr);
public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
protected:
	TableView* view() const;
	virtual void paintBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void paintForeground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}}

#endif // QF_QMLWIDGETS_TABLEITEMDELEGATE_H
