#ifndef CODECLASSRESULTSWIDGET_H
#define CODECLASSRESULTSWIDGET_H

#include <QWidget>

class QJsonObject;

namespace quickevent {  namespace core {
namespace og { class SqlTableModel; }
namespace si { class PunchRecord; }
}}

namespace Ui {
class CodeClassResultsWidget;
}

class CodeClassResultsWidget : public QWidget
{
	Q_OBJECT
private:
	using Super = QWidget;
public:
	explicit CodeClassResultsWidget(QWidget *parent = nullptr);
	~CodeClassResultsWidget() Q_DECL_OVERRIDE;

	void reloadDeferred();
	void reload();
	void onPunchReceived(const quickevent::core::si::PunchRecord &punch);

	static constexpr int ALL_CODES = 0;
	static constexpr int RESULTS_PUNCH_CODE = 1000;

	void reset(int class_id, int code, int pin_to_code = ALL_CODES);
	void loadSetup(const QJsonObject &jso);
	QJsonObject saveSetup();
protected:
	//void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
private:
	Ui::CodeClassResultsWidget *ui;
	quickevent::core::og::SqlTableModel *m_tableModel = nullptr;
	QTimer *m_reloadDeferredTimer = nullptr;
	int m_pinnedToCode = ALL_CODES;
};

#endif // CODECLASSRESULTSWIDGET_H
