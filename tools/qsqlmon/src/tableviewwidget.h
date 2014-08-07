#ifndef TABLEVIEWWIDGET_H
#define TABLEVIEWWIDGET_H

#include <QWidget>

namespace Ui {
class TableViewWidget;
}
namespace qf {
namespace qmlwidgets {
class TableView;
}
}
class TableViewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TableViewWidget(QWidget *parent = 0);
	~TableViewWidget();
public:
	qf::qmlwidgets::TableView* tableView();
	Q_SLOT void updateStatus();
	Q_SLOT void setInfo(const QString &info);

	Q_SIGNAL void statusBarAction(const QString &status_text);
private:

private:
	Ui::TableViewWidget *ui;
};

#endif // TABLEVIEWWIDGET_H
