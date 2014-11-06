#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

#include <qf/core/model/sqlquerytablemodel.h>
#include <qf/core/utils.h>
#include <qf/core/utils/table.h>
#include <qf/core/model/tablemodel.h>

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class Action;

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView, public framework::IPersistentSettings
{
	Q_OBJECT

	Q_ENUMS(RowEditorMode)
	Q_ENUMS(RowEditorMode)

	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(qf::core::model::TableModel* model READ tableModel WRITE setTableModel NOTIFY modelChanged)
	Q_PROPERTY(RowEditorMode rowEditorMode READ rowEditorMode WRITE setRowEditorMode NOTIFY rowEditorModeChanged)
	Q_PROPERTY(QString idColumnName READ idColumnName WRITE setIdColumnName)
private:
	typedef QTableView Super;
public:
	enum ActionGroup {ViewActions = 1, BlobActions = 2, SetValueActions = 4, RowActions = 8, CellActions = 16, SizeActions = 32,
					   CalculateActions = 64, ExportActions = 128, ImportActions = 256, SortActions = 512, SelectActions = 1024,
					   FilterActions = 2048, PasteActions = 4096, AllActions = 65535};
public:
	explicit TableView(QWidget *parent = 0);
	~TableView() Q_DECL_OVERRIDE;
public:
	enum RowEditorMode { EditRowsInline,
					EditRowsExternal,
					EditRowsMixed };
	enum RecordEditMode { ModeView = qf::core::model::TableModel::ModeView,
					   ModeEdit = qf::core::model::TableModel::ModeEdit,
					   ModeInsert = qf::core::model::TableModel::ModeInsert,
					   ModeCopy = qf::core::model::TableModel::ModeCopy,
					   ModeDelete = qf::core::model::TableModel::ModeDelete };

	QF_PROPERTY_IMPL2(RowEditorMode, r, R, owEditorMode, EditRowsInline)
	QF_PROPERTY_IMPL2(QString, i, I, dColumnName, QStringLiteral("id"))
public:
	qf::core::model::TableModel* tableModel() const;
	void setTableModel(qf::core::model::TableModel* m);
	Q_SIGNAL void modelChanged();

	Q_SLOT virtual void refreshActions();
	QList<Action*> toolBarActions() const {return m_toolBarActions;}

	Q_SLOT virtual void reload();

	Q_SLOT void insertRow();
	//! @param row_no if @a row_no < 0 than post current row.
	Q_SLOT void removeSelectedRows();
	Q_SLOT virtual bool postRow(int row_no = -1);
	//! discard all the row data changes.
	Q_SLOT virtual void revertRow(int row_no = -1);

	Q_SLOT void exportReport();

	/**
	* calls update viewport with rect clipping row \a row.
	* @param row if lower than 0 current row is updated.
	*/
	Q_SLOT void updateRow(int row = -1);
	Q_SLOT void updateDataArea();
	Q_SLOT void updateAll();

	//! If \a row_no < 0 row_no = current row.
	qf::core::utils::TableRow tableRow(int row_no = -1) const;
	qf::core::utils::TableRow selectedRow() const {return tableRow();}

	QList<int> selectedRowsIndexes() const;
	QList<int> selectedColumnsIndexes() const;

	Q_SIGNAL void editRowInExternalEditor(const QVariant &id, RecordEditMode mode);
private:
	Q_SIGNAL void searchStringChanged(const QString &str);
	qf::core::utils::Table::SortDef seekSortDefinition() const;
	int seekColumn() const;
	void seek(const QString &prefix_str);
	void cancelSeek();

	void exportReport_helper(const QVariant& _options);

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

	bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event) Q_DECL_OVERRIDE;
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

	virtual void insertRowInline();
	virtual void removeSelectedRowsInline();

	virtual void createActions();

	//static const int StandardContextMenuActionsGroups = AllActions & ~(SetValueActions | BlobActions | PasteActions);
	QList<Action*> standardContextMenuActions() { return contextMenuActionsForGroups(AllActions & ~(SetValueActions | BlobActions | PasteActions));}
	QList<Action*> contextMenuActions() { return m_contextMenuActions; }
	void setContextMenuActions(QList<Action*> lst) { m_contextMenuActions = lst; }
	QList<Action*> contextMenuActionsForGroups(int action_groups = AllActions);

	Action* action(const QString &act_oid);
	void enableAllActions(bool on);
protected:
	QString m_seekString;
	QMap<QString, Action*> m_actions;
	QMap<ActionGroup, Action*> m_separatorsForGroup;
	QMap<int, QStringList> m_actionGroups;
	QList<Action*> m_toolBarActions;
	QList<Action*> m_contextMenuActions;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
