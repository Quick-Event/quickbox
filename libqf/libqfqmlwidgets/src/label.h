#ifndef QF_QMLWIDGETS_LABEL_H
#define QF_QMLWIDGETS_LABEL_H

#include "qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QLabel>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Label : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(Qt::Alignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
	Q_PROPERTY(Qt::Alignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
private:
	typedef QLabel Super;
public:
	explicit Label(QWidget *parent = 0);
public:

	Qt::Alignment horizontalAlignment() const;
	void setHorizontalAlignment(Qt::Alignment al);
	Q_SIGNAL void horizontalAlignmentChanged(Qt::Alignment al);

	Qt::Alignment verticalAlignment() const;
	void setVerticalAlignment(Qt::Alignment al);
	Q_SIGNAL void verticalAlignmentChanged(Qt::Alignment al);

	QString text() const {return Super::text();}
	void setText(const QString &new_text);
	Q_SIGNAL void textChanged(const QString &new_text);
};

}
}

#endif // LABEL_H
