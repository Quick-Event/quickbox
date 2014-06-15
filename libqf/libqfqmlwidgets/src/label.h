#ifndef LABEL_H
#define LABEL_H

#include <QLabel>

namespace qf {
namespace qmlwidgets {

class Label : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
private:
	typedef QLabel Super;
public:
	explicit Label(QWidget *parent = 0);

	void setText(const QString &new_text);
	QString text() const {return Super::text();}
signals:
	void textChanged(const QString &new_text);
public slots:

};

}
}

#endif // LABEL_H
