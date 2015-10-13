#include "findregistrationedit.h"

#include "Competitors/competitorsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>

#include <QCompleter>
#include <QAbstractTableModel>

namespace {

static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}

class FilterModel : public QAbstractTableModel
{
private:
	typedef QAbstractTableModel Super;
public:
	FilterModel(QObject *parent);

	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE {Q_UNUSED(parent) return 1;}
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
private:
	qf::core::model::SqlTableModel *m_registrationsModel = nullptr;
};

FilterModel::FilterModel(QObject *parent)
	: Super(parent)
{
	m_registrationsModel = competitorsPlugin()->registrationsModel();
}

int FilterModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	const qf::core::utils::Table &table = m_registrationsModel->table();
	return table.rowCount();
}

QVariant FilterModel::data(const QModelIndex &index, int role) const
{
	const qf::core::utils::Table &table = m_registrationsModel->table();
	int row_no = index.row();
	if(row_no >= 0 && row_no < table.rowCount()) {
		if(role == Qt::EditRole || role == Qt::DisplayRole) {
			qf::core::utils::TableRow table_row = table.row(row_no);
			const qf::core::utils::Table::FieldList &fields = table.fields();
			static auto col_name = fields.fieldIndex(QStringLiteral("competitorName"));
			static int col_registration = fields.fieldIndex(QStringLiteral("registration"));
			static auto col_siid = fields.fieldIndex(QStringLiteral("siid"));
			static auto SI = QStringLiteral("SI:");
			return table_row.value(col_name).toString() + ' '
					+ table_row.value(col_registration).toString() + ' '
					+ SI + table_row.value(col_siid).toString();
		}
	}
	return QVariant();
}

}

FindRegistrationEdit::FindRegistrationEdit(QWidget *parent)
	: Super(parent)
{
	QCompleter *cmpl = new QCompleter(new FilterModel(this), this);
	cmpl->setCaseSensitivity(Qt::CaseInsensitive);
	cmpl->setFilterMode(Qt::MatchContains);
	setCompleter(cmpl);
}
