#ifndef QF_QMLWIDGETS_TABLEVIEWPROXYMODEL_H
#define QF_QMLWIDGETS_TABLEVIEWPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace qf {
namespace qmlwidgets {

class TableViewProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
private:
	typedef QSortFilterProxyModel Super;
public:
	TableViewProxyModel(QObject *parent = nullptr);
	~TableViewProxyModel() Q_DECL_OVERRIDE;
public:
	Q_SLOT void setRowFilterString(const QString &s);
	QString rowFilterString() const;
	bool isIdle() const;

	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
protected:
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
	bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const Q_DECL_OVERRIDE;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;
	int variantLessThan(const QVariant &left, const QVariant &right) const;
private:
	bool dataMatchFilter(const QVariant &d) const;
private:
	QByteArray m_rowFilterString;
	QVector<int> m_sortColumns;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEWPROXYMODEL_H
