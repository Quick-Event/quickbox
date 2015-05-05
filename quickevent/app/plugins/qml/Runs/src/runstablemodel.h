#ifndef RUNSTABLEMODEL_H
#define RUNSTABLEMODEL_H

#include <quickevent/og/sqltablemodel.h>

#include <qf/core/utils.h>

#include <QList>

class RunsTableModel : public quickevent::og::SqlTableModel
{
	Q_OBJECT
private:
	typedef quickevent::og::SqlTableModel Super;
public:
	RunsTableModel(QObject *parent = nullptr);

	QF_PROPERTY_IMPL(QList<int>, s, S, tartTimeHighlightRowsOrder)

	void setHighlightedClassId(int class_id);

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
private:
	int m_highlightedClassId = 0;
	int m_classStart;
	int m_classInterval;
};

#endif // RUNSTABLEMODEL_H
