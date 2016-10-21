#ifndef CODECLASSRESULTSWIDGET_H
#define CODECLASSRESULTSWIDGET_H

#include <QWidget>

class QJsonObject;

namespace quickevent {
namespace og { class SqlTableModel; }
namespace si { class PunchRecord; }
}

namespace Ui {
class CodeClassResultsWidget;
}

class CodeClassResultsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CodeClassResultsWidget(QWidget *parent = 0);
	~CodeClassResultsWidget() Q_DECL_OVERRIDE;

	void reload();
	void onPunchReceived(const quickevent::si::PunchRecord &punch);

	void loadSetup(const QJsonObject &jso);
	QJsonObject saveSetup();
private:
	Ui::CodeClassResultsWidget *ui;
	quickevent::og::SqlTableModel *m_tableModel = nullptr;
};

#endif // CODECLASSRESULTSWIDGET_H
