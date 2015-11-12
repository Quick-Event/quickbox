#ifndef QF_QMLWIDGETS_INTERNAL_TABLEVIEWCHOOSECOLUMNSWIDGET_H
#define QF_QMLWIDGETS_INTERNAL_TABLEVIEWCHOOSECOLUMNSWIDGET_H

#include <QWidget>
//#include <QVariantList>

class QStandardItemModel;

namespace qf {

namespace core {
namespace model {
class TableModel;
}
}

namespace qmlwidgets {
namespace internal {

namespace Ui {
class TableViewChooseColumnsWidget;
}

class TableViewChooseColumnsWidget : public QWidget
{
	Q_OBJECT
public:
	TableViewChooseColumnsWidget(QWidget *parent = NULL);
	~TableViewChooseColumnsWidget() Q_DECL_OVERRIDE;
protected:
	QStandardItemModel *f_exportedColumnsModel;
public:
	void loadColumns(qf::core::model::TableModel *model);
protected slots:
	void on_btColumnsAll_clicked();
	void on_btColumnsNone_clicked();
	void on_btColumnsInvert_clicked();
	void on_btColumnsAppendSelected_clicked();
	void on_btColumnsRemoveSelected_clicked();
	void on_btColumnShiftUp_clicked();
	void on_btColumnShiftDown_clicked();
public:
	/// [{caption:"",origin:("table" | "model"),index:(col_ix | fld_ix)}, ...]
	QVariantList exportedColumns() const;
	void setExportedColumns(const QVariant &cols);
private:
	Ui::TableViewChooseColumnsWidget *ui;
};

}}}

#endif // QF_QMLWIDGETS_INTERNAL_TABLEVIEWCHOOSECOLUMNSWIDGET_H

