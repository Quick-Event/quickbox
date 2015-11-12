#include "findrunneredit.h"

//#include <Competitors/competitorsplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/utils/table.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>

#include <QCompleter>
#include <QAbstractTableModel>
#include <QAbstractProxyModel>
/*
static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}
*/

class FindRunnersModel : public QAbstractTableModel
{
private:
	typedef QAbstractTableModel Super;
public:
	FindRunnersModel(QObject *parent);

	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {Q_UNUSED(parent) return 1;}
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	const qf::core::utils::Table& runnersTable() const {return m_runnersTable;}
	void setRunnersTable(const qf::core::utils::Table &t);
private:
	qf::core::utils::Table m_runnersTable;
};

FindRunnersModel::FindRunnersModel(QObject *parent)
	: Super(parent)
{
}

void FindRunnersModel::setRunnersTable(const qf::core::utils::Table &t)
{
	m_runnersTable = t;
}

int FindRunnersModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	const qf::core::utils::Table &table = runnersTable();
	return table.rowCount();
}

QVariant FindRunnersModel::data(const QModelIndex &index, int role) const
{
	const qf::core::utils::Table &table = runnersTable();
	int row_no = index.row();
	if(row_no >= 0 && row_no < table.rowCount()) {
		qf::core::utils::TableRow table_row = table.row(row_no);
		const qf::core::utils::Table::FieldList &fields = table.fields();
		static auto col_class_name = fields.fieldIndex(QStringLiteral("classes.name"));
		static auto col_name = fields.fieldIndex(QStringLiteral("competitorName"));
		static auto col_searchkey = fields.fieldIndex(QStringLiteral("competitorNameAscii7"));
		static int col_registration = fields.fieldIndex(QStringLiteral("registration"));
		static auto col_siid = fields.fieldIndex(QStringLiteral("siid"));
		static auto SI = QStringLiteral("SI:");
		if(role == Qt::DisplayRole) {
			return table_row.value(col_class_name).toString() + ' '
					+ table_row.value(col_name).toString() + ' '
					+ table_row.value(col_registration).toString() + ' '
					+ SI + table_row.value(col_siid).toString();
		}
		else if(role == Qt::EditRole) {
			return table_row.value(col_class_name).toString() + ' '
					+ table_row.value(col_searchkey).toString() + ' '
					+ table_row.value(col_registration).toString() + ' '
					+ SI + table_row.value(col_siid).toString() + ' '
					+ table_row.value(col_name).toString();
		}
	}
	return QVariant();
}

FindRunnerEdit::FindRunnerEdit(QWidget *parent)
	: Super(parent)
{	
}

void FindRunnerEdit::setTable(const qf::core::utils::Table &t)
{
	//QF_SAFE_DELETE(m_findRunnersModel);
	QF_SAFE_DELETE(m_completer);
	m_completer = new QCompleter(this);
	m_completer->setCaseSensitivity(Qt::CaseInsensitive);
	m_completer->setFilterMode(Qt::MatchContains);
	m_findRunnersModel = new FindRunnersModel(m_completer);
	m_findRunnersModel->setRunnersTable(t);
	m_completer->setModel(m_findRunnersModel);
	connect(m_completer, SIGNAL(activated(QModelIndex)), this, SLOT(onCompleterActivated(QModelIndex)));
	setCompleter(m_completer);
}

void FindRunnerEdit::onCompleterActivated(const QModelIndex &index)
{
	qfLogFuncFrame() << index << index.data();
	auto *proxy_model = qobject_cast<QAbstractProxyModel*>(completer()->completionModel());
	QF_ASSERT(proxy_model != nullptr, "Bat proxy model!", return);
	QModelIndex ix = proxy_model->mapToSource(index);
	const qf::core::utils::Table &table = m_findRunnersModel->runnersTable();
	int row_no = ix.row();
	if(row_no >= 0 && row_no < table.rowCount()) {
		qf::core::utils::TableRow table_row = table.row(row_no);
		emit runnerSelected(table_row.valuesMap(false));
	}
}
