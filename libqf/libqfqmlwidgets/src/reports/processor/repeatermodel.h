//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPEATERMODEL_H
#define QF_QMLWIDGETS_REPORTS_REPEATERMODEL_H

#include "../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>
#include <qf/core/utils/treetable.h>

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace reports {

class QFQMLWIDGETS_DECL_EXPORT RepeaterModel : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	typedef Qt::ItemDataRole DataRole;
public:
	explicit RepeaterModel(QObject *parent = 0);
public:
	virtual int rowCount() = 0;
	virtual int columnCount() = 0;
	virtual QVariant headerData(int col_no, DataRole = Qt::DisplayRole) = 0;
	virtual QVariant data(const QString &col_name, int row_no, DataRole = Qt::DisplayRole) = 0;
	virtual QVariant data(int col_no, int row_no, DataRole = Qt::DisplayRole) = 0;
	virtual QVariant table(const QString &table_name, int row_no) = 0;
public:
	static RepeaterModel* createFromData(const QVariant &data, QObject *parent = nullptr);
};

class RepeaterModelTreeTable : public RepeaterModel
{
	Q_OBJECT
private:
	typedef RepeaterModel Super;
public:
	explicit RepeaterModelTreeTable(QObject *parent = 0);
public:
	QF_PROPERTY_IMPL(qf::core::utils::TreeTable, t, T, reeTable)
public:
	int rowCount() Q_DECL_OVERRIDE;
	int columnCount() Q_DECL_OVERRIDE;
	QVariant headerData(int col_no, DataRole = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant data(const QString &col_name, int row_no, DataRole = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant data(int col_no, int row_no, DataRole = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant table(const QString &table_name, int row_no) Q_DECL_OVERRIDE;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPEATERMODEL_H
