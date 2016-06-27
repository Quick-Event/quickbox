#include "codeclassresultsgridwidget.h"
#include "codeclassresultswidget.h"

#include <qf/core/assert.h>

#include <QGridLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

CodeClassResultsGridWidget::CodeClassResultsGridWidget(QWidget *parent)
	: QWidget(parent)
{
	m_gridLayout = new QGridLayout(this);
	CodeClassResultsWidget *w = createClassResultsWidget();
	m_gridLayout->addWidget(w, 0, 0);
}

void CodeClassResultsGridWidget::addColumn()
{
	int cnt = columnCount();
	for (int i = 0; i < rowCount(); ++i) {
		CodeClassResultsWidget *w = createClassResultsWidget();
		m_gridLayout->addWidget(w, i, cnt);
	}
	m_columnCount++;
}

void CodeClassResultsGridWidget::removeColumn()
{
	int cnt = columnCount();
	if(cnt > 1) {
		cnt--;
		for (int i = 0; i < rowCount(); ++i) {
			QLayoutItem *it = m_gridLayout->itemAtPosition(i, cnt);
			if(it) {
				QWidget *w = it->widget();
				if(w)
					delete w;
			}
		}
		m_columnCount--;
	}
}

void CodeClassResultsGridWidget::addRow()
{
	int cnt = rowCount();
	for (int i = 0; i < columnCount(); ++i) {
		CodeClassResultsWidget *w = createClassResultsWidget();
		m_gridLayout->addWidget(w, cnt, i);
	}
	m_rowCount++;
}

void CodeClassResultsGridWidget::removeRow()
{
	int cnt = rowCount();
	if(cnt > 1) {
		cnt--;
		for (int i = 0; i < columnCount(); ++i) {
			QLayoutItem *it = m_gridLayout->itemAtPosition(cnt, i);
			if(it) {
				QWidget *w = it->widget();
				if(w)
					delete w;
			}
		}
		m_rowCount--;
	}
}

CodeClassResultsWidget *CodeClassResultsGridWidget::createClassResultsWidget()
{
	CodeClassResultsWidget *ret = new CodeClassResultsWidget(this);
	return ret;
}

CodeClassResultsWidget *CodeClassResultsGridWidget::resultsWidgetAt(int row, int col)
{
	QLayoutItem *it = m_gridLayout->itemAtPosition(row, col);
	if(it) {
		return qobject_cast<CodeClassResultsWidget*>(it->widget());
	}
	return nullptr;
}

static const auto PROP_ROW_COUNT = QStringLiteral("rowCount");
static const auto PROP_COL_COUNT = QStringLiteral("colCount");
static const auto PROP_CELLS = QStringLiteral("cells");

QByteArray CodeClassResultsGridWidget::saveLayout()
{
	QJsonObject jso;
	jso[PROP_ROW_COUNT] = rowCount();
	jso[PROP_COL_COUNT] = columnCount();
	QJsonArray cells;
	for (int j = 0; j < rowCount(); ++j) {
		for (int i = 0; i < columnCount(); ++i) {
			CodeClassResultsWidget *w = resultsWidgetAt(j, i);
			QF_ASSERT(w != nullptr, "Bad widget pos!", continue);
			cells.append(w->saveSetup());
		}
	}
	jso[PROP_CELLS] = cells;
	QJsonDocument jsd(jso);
	return jsd.toJson(QJsonDocument::Compact);
}

