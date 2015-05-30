#ifndef REGISTRATIONSWIDGET_H
#define REGISTRATIONSWIDGET_H

#include <QWidget>

namespace Ui {
class RegistrationsWidget;
}
namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}}
namespace qmlwidgets {
class TableView;
}
}

class RegistrationsWidget : public QWidget
{
	Q_OBJECT
public:
	enum class FocusWidget {Registration};
public:
	explicit RegistrationsWidget(QWidget *parent = 0);
	~RegistrationsWidget();

	void reload();
	Q_SLOT void onDbEvent(const QString &domain, const QVariant &payload);

	void setFocusToWidget(FocusWidget fw);
	qf::qmlwidgets::TableView* tableView();
private:
	Ui::RegistrationsWidget *ui;
	qf::core::model::SqlTableModel *m_registrationsModel;
};

#endif // REGISTRATIONSWIDGET_H
