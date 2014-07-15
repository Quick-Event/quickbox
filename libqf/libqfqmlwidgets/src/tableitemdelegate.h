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
	explicit TableItemDelegate(TableView *parent = 0);

public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
protected:
	TableView* view() const;
};

}}

#endif // QF_QMLWIDGETS_TABLEITEMDELEGATE_H
