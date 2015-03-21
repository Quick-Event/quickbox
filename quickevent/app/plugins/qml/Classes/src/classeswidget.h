#ifndef CLASSESWIDGET_H
#define CLASSESWIDGET_H

#include "thispartwidget.h"
#include <QFrame>

namespace Ui {
	class ClassesWidget;
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

class ClassesWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit ClassesWidget(QWidget *parent = 0);
	~ClassesWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private:
	Q_SLOT void import_ocad();
	Q_SLOT void reset();
	Q_SLOT void reload();
private:
	Ui::ClassesWidget *ui;
	qf::core::model::SqlTableModel *m_classesModel;
};

#endif // CLASSESWIDGET_H
