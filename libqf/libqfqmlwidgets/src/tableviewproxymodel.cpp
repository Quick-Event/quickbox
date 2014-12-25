#include "tableviewproxymodel.h"

#include <qf/core/log.h>
#include <qf/core/collator.h>

#include <QColor>
#include <QTextCodec>

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
	QByteArray ba = qf::core::Collator::toAscii7(s, true);
	qfDebug() << ba;
	if(ba == m_rowFilterString)
		return;
	m_rowFilterString = ba;
	invalidateFilter();
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
/*
QTextCodec *TableViewProxyModel::tcASCII7()
{
	static QTextCodec *tc_ascii7 = NULL;
	if(!tc_ascii7) {
		tc_ascii7 = QTextCodec::codecForName("ASCII7");
		qfInfo() << "LOADED codec:" << tc_ascii7;
		if(!tc_ascii7)
			qfFatal("Can't load ASCII7 codec.");
	}
	return tc_ascii7;
}
*/
bool TableViewProxyModel::dataMatchFilter(const QVariant &d) const
{
	QString s = d.toString();
	QByteArray ba = qf::core::Collator::toAscii7(s, true);
	bool ret = ba.contains(m_rowFilterString);
	return ret;
}

