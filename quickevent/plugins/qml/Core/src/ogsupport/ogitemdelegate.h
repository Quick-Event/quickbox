#ifndef OGITEMDELEGATE_H
#define OGITEMDELEGATE_H

#include <QStyledItemDelegate>

class OGItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
private:
	typedef QStyledItemDelegate Super;
public:
	OGItemDelegate(QObject * parent = nullptr);
	~OGItemDelegate() Q_DECL_OVERRIDE;
};

#endif // OGITEMDELEGATE_H
