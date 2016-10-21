#ifndef CODECLASSRESULTSGRIDWIDGET_H
#define CODECLASSRESULTSGRIDWIDGET_H

#include <QWidget>

namespace quickevent { namespace si { class PunchRecord; }}

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

	Q_SIGNAL void punchReceived(const quickevent::si::PunchRecord &punch);
	void onPunchReceived(const quickevent::si::PunchRecord &punch);

	void loadLayout(const QByteArray &data);
	QByteArray saveLayout();
private:
	CodeClassResultsWidget* resultsWidgetAt(int row, int col);
private:
	CodeClassResultsWidget* createClassResultsWidget();
	QGridLayout* m_gridLayout;
	int m_rowCount = 1;
	int m_columnCount = 1;
};

#endif // CODECLASSRESULTSGRIDWIDGET_H
