#ifndef SPEAKERWIDGET_H
#define SPEAKERWIDGET_H

#include <QFrame>

namespace Ui {
class SpeakerWidget;
}
namespace qf {
//namespace core { namespace model { class SqlTableModel; } }
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

namespace quickevent { namespace og { class SqlTableModel; }}
namespace quickevent { namespace si { class PunchRecord; }}

class ThisPartWidget;

class SpeakerWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit SpeakerWidget(QWidget *parent = 0);
	~SpeakerWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);

	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	Q_SIGNAL void punchReceived(const quickevent::si::PunchRecord &punch);
private:
	//Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	void updateTableView(int punch_id);

	void loadSettings();
	void saveSettings();

	bool isPartActive();

	void onCodeClassActivated(int class_id, int code);
private:
	Ui::SpeakerWidget *ui;
	quickevent::og::SqlTableModel *m_punchesModel = nullptr;
	ThisPartWidget *m_partWidget = nullptr;
	bool m_resetRequest = false;
	bool m_settingsLoaded = false;
};

#endif // SPEAKERWIDGET_H
