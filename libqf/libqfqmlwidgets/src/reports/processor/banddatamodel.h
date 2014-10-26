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
	explicit BandDataModel(QObject *parent = 0);
public:
	virtual int rowCount() = 0;
	virtual int columnCount() = 0;
	virtual QVariant headerData(int col_no, DataRole role = Qt::DisplayRole) = 0;
	virtual QVariant data(int row_no, const QString &col_name, DataRole role = Qt::DisplayRole) = 0;
	virtual QVariant data(int row_no, int col_no, DataRole role = Qt::DisplayRole) = 0;
	virtual QVariant table(int row_no, const QString &table_name);
	virtual QString dump() const {return QString();}
public:
	static BandDataModel* createFromData(const QVariant &data, QObject *parent = nullptr);
};

class TreeTableBandDataModel : public BandDataModel
{
	Q_OBJECT
private:
	typedef BandDataModel Super;
public:
	explicit TreeTableBandDataModel(QObject *parent = 0);
public:
	QF_PROPERTY_IMPL(qf::core::utils::TreeTable, t, T, reeTable)
public:
	int rowCount() Q_DECL_OVERRIDE;
	int columnCount() Q_DECL_OVERRIDE;
	QVariant headerData(int col_no, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant data(int row_no, int col_no, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant data(int row_no, const QString &col_name, DataRole role = Qt::DisplayRole) Q_DECL_OVERRIDE;
	QVariant table(int row_no, const QString &table_name) Q_DECL_OVERRIDE;
	QString dump() const Q_DECL_OVERRIDE;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_BANDDTAMODEL_H
