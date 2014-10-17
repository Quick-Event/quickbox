//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMREPEATER_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMREPEATER_H

#include "reportitemframe.h"
#include "../../qmlwidgetsglobal.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

class RepeaterModel;

class QFQMLWIDGETS_DECL_EXPORT ReportItemRepeater : public ReportItemFrame
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::reports::ReportItemFrame* header READ header WRITE setHeader NOTIFY headerChanged)
	Q_PROPERTY(bool headerOnBreak READ isHeaderOnBreak WRITE setHeaderOnBreak NOTIFY headerOnBreakChanged)
	Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
private:
	typedef ReportItemFrame Super;
public:
	ReportItemRepeater(ReportItem *parent = nullptr);
	~ReportItemRepeater() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_OBJECT_IMPL(ReportItemFrame*, h, H, eader)
	QF_PROPERTY_BOOL_IMPL(h, H, eaderOnBreak)
	QF_PROPERTY_IMPL(QVariant, m, M, odel)
	QF_PROPERTY_IMPL(int, c, C, urrentIndex)
protected:
	RepeaterModel* dataModel();
	void createDataModel(const QVariant &data_source);
public:
	Q_INVOKABLE QVariant data(const QString &field_name, int row_no = -1);
public:
	//--virtual void resetIndexToPrintRecursively(bool including_para_texts);
	PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect) Q_DECL_OVERRIDE;
	virtual PrintResult printHtml(HTMLElement &out);

	//--qf::core::utils::TreeTable dataTable();
	//--qf::core::utils::TreeTableRow dataRow();
protected:
	RepeaterModel *m_dataModel = nullptr;
};

#if 0
//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemDetail : public ReportItemFrame
{
	Q_OBJECT
	friend class ReportItemBand;
private:
	typedef ReportItemFrame Super;
protected:
	//qf::core::utils::TreeTableRow f_dataRow;
	int f_currentRowNo;

	virtual ReportItemDetail* toDetail() {return this;}
public:
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printHtml(HTMLElement &out);

	qf::core::utils::TreeTable dataTable();
	qf::core::utils::TreeTableRow dataRow();
	/// cislo prave tisteneho radku, pocitano od nuly.
	int currentRowNo() const {return f_currentRowNo;}
	void resetCurrentRowNo() {f_currentRowNo = 0;}
public:
	ReportItemDetail(ReportItem *parent = nullptr);
	virtual ~ReportItemDetail() {}
};

//! TODO: write class documentation.
class QFQMLWIDGETS_DECL_EXPORT ReportItemBand : public ReportItemFrame
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
	//TODO: introduce dataModel property as ReportBandModel : public QObject ancestor implementing TreeTable interface
	//      Then data property can be removed
	Q_PROPERTY(qf::core::utils::TreeTable data READ data WRITE setData NOTIFY dataChanged)
	Q_PROPERTY(bool headerOnBreak READ isHeaderOnBreak WRITE setHeaderOnBreak NOTIFY headerOnBreakChanged)
	/*
	Q_PROPERTY(ReportItemFrame* header READ header WRITE setHeader NOTIFY headerChanged)
	Q_PROPERTY(ReportItemDetail* detail READ detail WRITE setDetail NOTIFY detailChanged)
	Q_PROPERTY(ReportItemFrame* footer READ footer WRITE setFooter NOTIFY footerChanged)
	*/
private:
	typedef ReportItemFrame Super;
public:
	ReportItemBand(ReportItem *parent = nullptr);
	~ReportItemBand() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QString, d, D, ataSource)
	QF_PROPERTY_IMPL(qf::core::utils::TreeTable, d, D, ata)
	QF_PROPERTY_BOOL_IMPL(h, H, eaderOnBreak)
	/*
	QF_PROPERTY_OBJECT_IMPL(ReportItemFrame*, h, H, eader)
	QF_PROPERTY_OBJECT_IMPL(ReportItemDetail*, d, D, etail)
	QF_PROPERTY_OBJECT_IMPL(ReportItemFrame*, f, F, ooter)
	*/
public:
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);

	virtual  ReportItemBand* toBand()  {return this;}
	ReportItemDetail* detail();

	virtual qf::core::utils::TreeTable dataTable();
	virtual void resetIndexToPrintRecursively(bool including_para_texts);
protected:
	qf::core::utils::TreeTable f_dataTable;
	bool dataTableLoaded;
};
#endif

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMREPEATER_H
