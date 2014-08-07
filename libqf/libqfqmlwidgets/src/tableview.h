#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

#include <qf/core/model/sqlquerytablemodel.h>
#include <qf/core/utils/table.h>

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class Action;

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(qf::core::model::TableModel* model READ tableModel WRITE setTableModel NOTIFY modelChanged)
	Q_PROPERTY(bool editRowsInline READ isEditRowsInline WRITE setEditRowsInline NOTIFY editRowsInlineChanged)
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
	qf::core::model::TableModel* tableModel() const;
	void setTableModel(qf::core::model::TableModel* m);
	Q_SIGNAL void modelChanged();

	Q_SLOT virtual void refreshActions();
	QList<Action*> toolBarActions() const {return m_toolBarActions;}

	Q_SLOT virtual void reload();

	Q_SLOT void insertRow();
	//! @param row_no if @a row_no < 0 than post current row.
	Q_SLOT virtual bool postRow(int row_no = -1);
	//! discard all the row data changes.
	Q_SLOT virtual void revertRow(int row_no = -1);
	/**
	* calls update viewport with rect clipping row \a row.
	* @param row if lower than 0 current row is updated.
	*/
	Q_SLOT void updateRow(int row = -1);

	bool isEditRowsInline() const { return m_editRowsInline; }
	void setEditRowsInline(bool arg)
	{
		if (m_editRowsInline != arg) {
			m_editRowsInline = arg;
			emit editRowsInlineChanged(arg);
		}
	}
	Q_SIGNAL void editRowsInlineChanged(bool arg);

	Q_SIGNAL void editRowInExternalEditor(const QVariant &id, qf::core::model::TableModel::RowEditMode mode);
private:
	Q_SIGNAL void searchStringChanged(const QString &str);
	qf::core::utils::Table::SortDef seekSortDefinition() const;
	int seekColumn() const;
	void seek(const QString &prefix_str);
	void cancelSeek();

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void currentChanged(const QModelIndex& current, const QModelIndex& previous) Q_DECL_OVERRIDE;

	virtual void insertRowInline();

	virtual void createActions();
	QList<Action*> contextMenuActionsForGroups(int action_groups = AllActions);
	Action* action(const QString &act_oid);
	void enableAllActions(bool on);
protected:
	QString m_seekString;
	QMap<QString, Action*> m_actions;
	QMap<ActionGroup, Action*> m_separatorsForGroup;
	QMap<int, QStringList> m_actionGroups;
	QList<Action*> m_toolBarActions;
	bool m_editRowsInline;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
