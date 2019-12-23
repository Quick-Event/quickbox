#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMDETAIL_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMDETAIL_H

#include "reportitemframe.h"
#include "../../qmlwidgetsglobal.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

class QFQMLWIDGETS_DECL_EXPORT ReportItemDetail : public ReportItemFrame
{
	Q_OBJECT
	Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
private:
	typedef ReportItemFrame Super;
public:
	QF_PROPERTY_IMPL2(int, c, C, urrentIndex, -1)
public:
	ReportItemDetail(ReportItem *parent = nullptr);
	~ReportItemDetail() Q_DECL_OVERRIDE;
public:
	PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect) Q_DECL_OVERRIDE;
	PrintResult printHtml(HTMLElement &out) Q_DECL_OVERRIDE;

	void resetCurrentIndex();
	int rowCount();
	Q_SIGNAL void rowCountChanged();

	Q_INVOKABLE QVariant data(int row_no, const QString &field_name, int role = Qt::DisplayRole);
	Q_INVOKABLE QVariant rowData(const QString &field_name, int role = Qt::DisplayRole);
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMDETAIL_H
