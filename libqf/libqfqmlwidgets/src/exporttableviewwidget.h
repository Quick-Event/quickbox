#ifndef QF_QMLWIDGETS_EXPORTTABLEVIEWWIDGET_H
#define QF_QMLWIDGETS_EXPORTTABLEVIEWWIDGET_H

#include "framework/dialogwidget.h"
#include "framework/ipersistentoptions.h"

#include <QWidget>

class QTableView;

namespace qf {
//namespace core { namespace model { class TableModel; }}
namespace qmlwidgets {

namespace Ui {
class ExportTableViewWidget;
}

class ExportTableViewWidget : public qf::qmlwidgets::framework::DialogWidget, public qf::qmlwidgets::framework::IPersistentOptions
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit ExportTableViewWidget(QTableView *table_view, QWidget *parent = nullptr);
	~ExportTableViewWidget() Q_DECL_OVERRIDE;

	Q_SIGNAL void exportRequest(const QVariant &options);

	QVariant persistentOptions() Q_DECL_OVERRIDE;
	void setPersistentOptions(const QVariant &opts) Q_DECL_OVERRIDE;

	Q_SLOT void loadPersistentOptions();
	Q_SLOT void applyOptions();
protected:
	virtual QVariant exportOptions() const;
	virtual void setExportOptions(const QVariant& opts) const;
protected:
	Ui::ExportTableViewWidget *ui;
};

} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_EXPORTTABLEVIEWWIDGET_H
