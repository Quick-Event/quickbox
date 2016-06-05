#ifndef CODECLASSRESULTSGRIDWIDGET_H
#define CODECLASSRESULTSGRIDWIDGET_H

#include <QWidget>

class CodeClassResultsWidget;
class QGridLayout;

class CodeClassResultsGridWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CodeClassResultsGridWidget(QWidget *parent = 0);

	int columnCount() {return m_columnCount;}
	int rowCount() {return m_rowCount;}

	void addColumn();
	void removeColumn();
	void addRow();
	void removeRow();
private:
	CodeClassResultsWidget* createClassResultsWidget();
	QGridLayout* m_gridLayout;
	int m_rowCount = 1;
	int m_columnCount = 1;
};

#endif // CODECLASSRESULTSGRIDWIDGET_H
