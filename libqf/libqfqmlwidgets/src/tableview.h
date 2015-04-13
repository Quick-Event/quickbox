#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

//#include <qf/core/model/sqltablemodel.h>
#include <qf/core/utils.h>
#include <qf/core/utils/table.h>
#include <qf/core/model/datadocument.h>

#include <QTableView>

class QAbstractProxyModel;

namespace qf {
namespace qmlwidgets {

class Action;
class TableViewProxyModel;

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView, public framework::IPersistentSettings
{
	Q_OBJECT

	Q_ENUMS(InlineEditStrategy)
	Q_ENUMS(RowEditorMode)
	Q_ENUMS(RecordEditMode)

	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(qf::core::model::TableModel* model READ tableModel WRITE setTableModel NOTIFY tableModelChanged)
	Q_PROPERTY(RowEditorMode rowEditorMode READ rowEditorMode WRITE setRowEditorMode NOTIFY rowEditorModeChanged)
	Q_PROPERTY(InlineEditStrategy inlineEditStrategy READ inlineEditStrategy WRITE setInlineEditStrategy NOTIFY inlineEditStrategyChanged)
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
	enum InlineEditStrategy {
		OnCurrentRowChange,
		OnEditedValueCommit
	};
	enum RowEditorMode {
		EditRowsInline,
		EditRowsExternal,
		EditRowsMixed
	};
	enum RecordEditMode {
		ModeView = qf::core::model::DataDocument::ModeView,
		ModeEdit = qf::core::model::DataDocument::ModeEdit,
		ModeInsert = qf::core::model::DataDocument::ModeInsert,
		ModeCopy = qf::core::model::DataDocument::ModeCopy,
		ModeDelete = qf::core::model::DataDocument::ModeDelete
	};

	QF_PROPERTY_IMPL2(InlineEditStrategy, i, I, nlineEditStrategy, OnCurrentRowChange)
	QF_PROPERTY_IMPL2(RowEditorMode, r, R, owEditorMode, EditRowsInline)
	QF_PROPERTY_IMPL2(QString, i, I, dColumnName, QStringLiteral("id"))
public:
	qf::core::model::TableModel* tableModel() const;
	void setTableModel(qf::core::model::TableModel* m);
	Q_SIGNAL void tableModelChanged();
	Q_SIGNAL void currentRowChanged(int current_row);

	Q_SLOT virtual void refreshActions();
	QList<Action*> toolBarActions() const {return m_toolBarActions;}

	Q_SLOT void resetColumnsSettings();

	Q_SLOT virtual void reload();

	Q_SLOT virtual void insertRow();
	//! @param row_no if @a row_no < 0 than post current row.
	Q_SLOT void removeSelectedRows();
	Q_SLOT virtual bool postRow(int row_no = -1);
	//! discard all the row data changes.
	Q_SLOT virtual void revertRow(int row_no = -1);

	Q_SLOT virtual void cloneRowInline();
	Q_SLOT virtual void cloneRow();

	Q_SLOT virtual void copy();
	Q_SLOT virtual void copySpecial();
	Q_SLOT virtual void paste();

	Q_SLOT void setValueInSelection();
	Q_SLOT void setNullInSelection() {setValueInSelection_helper(QVariant());}

	Q_SLOT void editCellContentInEditor();

	Q_SLOT void exportReport();

	/**
	* calls update viewport with rect clipping row \a row.
	* @param row if lower than 0 current row is updated.
	*/
	Q_SLOT void updateRow(int row = -1);
	Q_SLOT void updateDataArea();
	Q_SLOT void updateAll();

	//! If \a row_no < 0 row_no = current row.
	qf::core::utils::TableRow& tableRowRef(int row_no = -1);
	qf::core::utils::TableRow tableRow(int row_no = -1) const;
	qf::core::utils::TableRow selectedRow() const {return tableRow();}

	QList<int> selectedRowsIndexes() const;
	QList<int> selectedColumnsIndexes() const;

	Q_SIGNAL void editRowInExternalEditor(const QVariant &id, int mode);
	Q_SLOT virtual void rowExternallySaved(const QVariant &id, int mode);

	Q_SIGNAL void filterDialogRequest();
	Q_SLOT void filterByString(const QString &s);

	Q_SLOT void setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate) {Super::setItemDelegateForColumn(column, delegate);}
private:
	Q_SIGNAL void seekStringChanged(const QString &str);
	//qf::core::utils::Table::SortDef seekSortDefinition() const;
	int seekColumn() const;
	void seek(const QString &prefix_str);
	void cancelSeek();

	QModelIndex toTableModelIndex(const QModelIndex &table_view_index) const;
	int toTableModelRowNo(int table_view_row_no) const;

	void exportReport_helper(const QVariant& _options);

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

	bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event) Q_DECL_OVERRIDE;
	void commitData(QWidget *editor) Q_DECL_OVERRIDE;
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

	virtual void insertRowInline();
	virtual void removeSelectedRowsInline();

	virtual void createActions();

	void setValueInSelection_helper(const QVariant &new_val);
	void copySpecial_helper(const QString &fields_separator, const QString &rows_separator, const QString &field_quotes, bool replace_escapes);

	//static const int StandardContextMenuActionsGroups = AllActions & ~(SetValueActions | BlobActions | PasteActions);
	QList<Action*> standardContextMenuActions() { return contextMenuActionsForGroups(AllActions);}
	QList<Action*> contextMenuActions() { return m_contextMenuActions; }
	void setContextMenuActions(QList<Action*> lst) { m_contextMenuActions = lst; }
	QList<Action*> contextMenuActionsForGroups(int action_groups = AllActions);

	Action* action(const QString &act_oid);
	void enableAllActions(bool on);

	QAbstractProxyModel* lastProxyModel() const;
private:
	/// hide this function do disable filter proxymodel bypassing
	void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
protected:
	QString m_seekString;
	QMap<QString, Action*> m_actions;
	QMap<ActionGroup, Action*> m_separatorsForGroup;
	QMap<int, QStringList> m_actionGroups;
	QList<Action*> m_toolBarActions;
	QList<Action*> m_contextMenuActions;
	TableViewProxyModel *m_proxyModel;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
