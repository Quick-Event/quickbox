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
	explicit HeaderView(Qt::Orientation orientation, QWidget *parent = 0);
	~HeaderView() Q_DECL_OVERRIDE;

	Q_SIGNAL void sortColumnAdded(int ix);
	Q_SLOT void setSeekString(const QString &str);
protected:
	void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) override;
protected:
	QLabel *m_seekStringLabel;
};

}}

#endif // HEADERVIEW_H
