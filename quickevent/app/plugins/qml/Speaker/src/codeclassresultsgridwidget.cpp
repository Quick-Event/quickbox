#include "codeclassresultsgridwidget.h"
#include "codeclassresultswidget.h"

#include <QGridLayout>

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
