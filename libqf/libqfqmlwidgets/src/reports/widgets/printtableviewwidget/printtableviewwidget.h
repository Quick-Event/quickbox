#ifndef QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H
#define QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H

#include "../../../framework/dialogwidget.h"
#include "../../../framework/ipersistentoptions.h"

#include <QVariant>

namespace qf {
namespace qmlwidgets {
class TableView;
namespace reports {

namespace Ui {
class PrintTableViewWidget;
}

class PrintTableViewWidget : public qf::qmlwidgets::framework::DialogWidget, public qf::qmlwidgets::framework::IPersistentOptions
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit PrintTableViewWidget(TableView *table_view, QWidget *parent = 0);
	~PrintTableViewWidget() Q_DECL_OVERRIDE;
public:
	QString reportFileName() const;
	QString reportTitle() const;
	QString reportNote() const;
	bool isSelectedRowsOnly() const;

	QVariant persistentOptions() Q_DECL_OVERRIDE;
	void setPersistentOptions(const QVariant &opts) Q_DECL_OVERRIDE;

	Q_SLOT void loadPersistentOptions();
	Q_SLOT void applyOptions();

	Q_SIGNAL void printRequest(const QVariant &options);
private:
	QVariant exportOptions() const;
	void setExportOptions(const QVariant& opts) const;
private:
	Ui::PrintTableViewWidget *ui;
};

}}}

#endif // QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H
