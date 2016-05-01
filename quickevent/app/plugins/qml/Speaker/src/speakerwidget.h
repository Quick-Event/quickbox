#ifndef SPEAKERWIDGET_H
#define SPEAKERWIDGET_H

#include <QFrame>

namespace Ui {
class SpeakerWidget;
}
namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

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
private:
	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();
private:
	Ui::SpeakerWidget *ui;
	qf::core::model::SqlTableModel *m_punchesModel;
	//qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
};

#endif // SPEAKERWIDGET_H
