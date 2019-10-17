//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_BANDDTAMODEL_H
#define QF_QMLWIDGETS_REPORTS_BANDDTAMODEL_H

#include "../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>
#include <qf/core/utils/treetable.h>

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace reports {

class QFQMLWIDGETS_DECL_EXPORT BandDataModel : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	typedef Qt::ItemDataRole DataRole;
public:
	explicit BandDataModel(QObject *parent = nullptr);
	~BandDataModel() Q_DECL_OVERRIDE;

	QF_PROPERTY_BOOL_IMPL2(d, D, ataValid, false)
public:
	Q_INVOKABLE virtual int rowCount() {return 0;}
	Q_INVOKABLE virtual int columnCount() {return 0;}
	Q_INVOKABLE virtual QVariant tableData(const QString &key, DataRole role = Qt::DisplayRole) {Q_UNUSED(key) Q_UNUSED(role) return QVariant();}
	Q_INVOKABLE virtual QVariant headerData(int col_no, DataRole role = Qt::DisplayRole) {Q_UNUSED(col_no) Q_UNUSED(role) return QVariant();}
	Q_INVOKABLE virtual QString columnNameToIndex(const QString &column_name) {Q_UNUSED(column_name) return QString();}
	// cannot use arg overloading because of QML
	Q_INVOKABLE virtual QVariant dataByIndex(int row_no, int col_no, DataRole role = Qt::DisplayRole) {Q_UNUSED(row_no) Q_UNUSED(col_no) Q_UNUSED(role) return QVariant();}
	Q_INVOKABLE virtual QVariant dataByName(int row_no, const QString &col_name, DataRole role = Qt::DisplayRole) {Q_UNUSED(row_no) Q_UNUSED(col_name) Q_UNUSED(role) return QVariant();}
	virtual QVariant table(int row_no, const QString &table_name);
	Q_INVOKABLE virtual QString dump() const {return QString();}

	Q_SLOT void invalidateData() {setDataValid(false);}
public:
	static BandDataModel* createFromData(const QVariant &dataByIndex, QObject *parent = nullptr);
};

class TreeTableBandDataModel : public BandDataModel
{
	Q_OBJECT
private:
	typedef BandDataModel Super;
public:
	explicit TreeTableBandDataModel(QObject *parent = nullptr);
public:
	int rowCount() Q_DECL_OVERRIDE;
	int columnCount() Q_DECL_OVERRIDE;
	QVariant tableData(const QString &key, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant headerData(int col_no, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant dataByIndex(int row_no, int col_no, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant dataByName(int row_no, const QString &col_name, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant table(int row_no, const QString &table_name) Q_DECL_OVERRIDE;
	QString dump() const Q_DECL_OVERRIDE;

	const qf::core::utils::TreeTable& treeTable() const;
	void setTreeTable(const qf::core::utils::TreeTable &tree_table);
private:
	qf::core::utils::TreeTable m_treeTable;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_BANDDTAMODEL_H
