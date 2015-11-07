#ifndef QF_QMLWIDGETS_EXPORTCSVTABLEVIEWWIDGET_H
#define QF_QMLWIDGETS_EXPORTCSVTABLEVIEWWIDGET_H

#include "exporttableviewwidget.h"

namespace qf {
namespace qmlwidgets {

class ExportCsvTableViewWidget : public qf::qmlwidgets::ExportTableViewWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::ExportTableViewWidget Super;
public:
	ExportCsvTableViewWidget(QTableView *table_view, QWidget *parent = nullptr);
protected:
	QVariant exportOptions() const Q_DECL_OVERRIDE;
	void setExportOptions(const QVariant& opts) const Q_DECL_OVERRIDE;
};

} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_EXPORTCSVTABLEVIEWWIDGET_H
