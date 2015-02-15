#ifndef CARDREADOUTWIDGET_H
#define CARDREADOUTWIDGET_H

#include <QFrame>

namespace Ui {
	class CardReadoutWidget;
}

namespace qf {
namespace qmlwidgets {
class Action;
}
}

class CardReadoutWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit CardReadoutWidget(QWidget *parent = 0);
	~CardReadoutWidget() Q_DECL_OVERRIDE;
private:
	void createActions();
	//qf::qmlwidgets::Action* action(const QString &oid);
private:
	Ui::CardReadoutWidget *ui;
	qf::qmlwidgets::Action *m_actCommOpen = nullptr;
	//qf::qmlwidgets::Action *m_actSqlConnect = nullptr;
};

#endif // CARDREADOUTWIDGET_H
