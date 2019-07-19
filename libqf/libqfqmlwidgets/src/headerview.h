#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include "qmlwidgetsglobal.h"

#include <QHeaderView>

class QLabel;

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT HeaderView : public QHeaderView
{
	Q_OBJECT
private:
	typedef QHeaderView Super;
public:
	explicit HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);
	~HeaderView() Q_DECL_OVERRIDE;

	Q_SIGNAL void sortColumnAdded(int ix);
	Q_SLOT void setSeekString(const QString &str);
protected:
	void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) override;
	void paintSection(QPainter *painter, const QRect &section_rect, int logical_index) const override;
protected:
	QLabel *m_seekStringLabel;
	QVector<int> m_extraSortColumns;
};

}}

#endif // HEADERVIEW_H
