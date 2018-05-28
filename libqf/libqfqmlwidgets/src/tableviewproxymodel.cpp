#include "tableviewproxymodel.h"

#include <qf/core/log.h>
#include <qf/core/collator.h>

#include <QColor>
#include <QTextCodec>
#include <QDateTime>

//namespace qfm = qf::core::model;
using namespace qf::qmlwidgets;

TableViewProxyModel::TableViewProxyModel(QObject *parent)
	: Super(parent)
{

}

TableViewProxyModel::~TableViewProxyModel()
{

}

void TableViewProxyModel::setRowFilterString(const QString &s)
{
	qfLogFuncFrame() << s;
	QByteArray ba = qf::core::Collator::toAscii7(QLocale::Czech, s, true);
	qfDebug() << ba;
	if(ba == m_rowFilterString)
		return;
	m_rowFilterString = ba;
	invalidateFilter();
}

QString TableViewProxyModel::rowFilterString() const
{
	return m_rowFilterString;
}

bool TableViewProxyModel::isIdle() const
{
	return m_rowFilterString.isEmpty() && sortColumn() < 0;
}

void TableViewProxyModel::sort(int column, Qt::SortOrder order)
{
	m_sortColumns.clear();
	if(order == Qt::AscendingOrder)
		m_sortColumns << column;
	Super::sort(column, order);
}

QVariant TableViewProxyModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
	if(!m_rowFilterString.isEmpty()) {
		if(role == Qt::BackgroundRole) {
			if(dataMatchFilter(data(index)))
				ret = QColor(Qt::yellow);
		}
	}
	return ret;
}

QVariant TableViewProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical && role == Qt::DisplayRole)
		return section + 1;
	return Super::headerData(section, orientation, role);
}

bool TableViewProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(m_rowFilterString.isEmpty())
		return true;
	QAbstractItemModel *sm = sourceModel();
	for(int i=0; i<sm->columnCount(); i++) {
		QModelIndex ix = sm->index(source_row, i, source_parent);
		if(dataMatchFilter(sm->data(ix)))
			return true;
	}
	return false;
}

int TableViewProxyModel::variantLessThan(const QVariant &left, const QVariant &right) const
{
	if(left.userType() == qMetaTypeId<QString>() && right.userType() == qMetaTypeId<QString>()) {
		const QByteArray lb = qf::core::Collator::toAscii7(QLocale::Czech, left.toString(), true);
		const QByteArray rb = qf::core::Collator::toAscii7(QLocale::Czech, right.toString(), true);
		int lsz = lb.size();
		int rsz = rb.size();
		for(int i=0; ; i++) {
			char lc = (i<lsz)? lb.at(i): 0;
			char rc = (i<rsz)? rb.at(i): 0;
			if(lc == rc) {
				if(lc == 0) {
					/// same
					return false;
				}
			}
			else {
				return (lc < rc);
			}
		}
	}
	if(!left.isValid()) {
		return right.isValid();
	}
	if(!right.isValid()) {
		return false;
	}
	if(left.isNull()) {
		return !right.isNull();
	}
	if(right.isNull()) {
		return false;
	}
	/*
	else {
		switch (left.userType()) {
		case QVariant::Int:
			return left.toInt() - right.toInt();
		case QVariant::UInt:
			return left.toUInt() - right.toUInt();
		case QVariant::LongLong:
			return left.toLongLong() - right.toLongLong();
		case QVariant::ULongLong:
			return left.toULongLong() - right.toULongLong();
		case QMetaType::Float:
			return left.toFloat() - right.toFloat();
		case QVariant::Double:
			return left.toDouble() - right.toDouble();
		case QVariant::Char:
			return left.toChar().unicode() - right.toChar().unicode();
		case QVariant::Date:
			return left.toDate().toJulianDay() - right.toDate().toJulianDay();
		case QVariant::Time:
			return left.toTime().msecsSinceStartOfDay() - right.toTime().msecsSinceStartOfDay();
		case QVariant::DateTime:
			return left.toDateTime().toMSecsSinceEpoch() - right.toDateTime().toMSecsSinceEpoch();
		default:
			return left.toString().compare(right.toString());
		}
	}
		*/
	return (left < right);
}

bool TableViewProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	//qfInfo() << left << right;
	bool ret = false;
	const QAbstractItemModel *source_model = sourceModel();
	if(source_model) {
		QVariant lv = source_model->data(left, Qt::EditRole); /// comparing display role is not working for NULL values
		QVariant rv = source_model->data(right, Qt::EditRole);
		ret = variantLessThan(lv, rv);
		//qfInfo() << lv << "vs" << rv << "->" << ret;
	}
	return ret;
}

bool TableViewProxyModel::dataMatchFilter(const QVariant &d) const
{
	QString s = d.toString();
	QByteArray ba = qf::core::Collator::toAscii7(QLocale::Czech, s, true);
	bool ret = ba.contains(m_rowFilterString);
	return ret;
}

