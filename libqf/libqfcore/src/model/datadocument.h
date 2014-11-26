#ifndef QF_CORE_MODEL_DATADOCUMENT_H
#define QF_CORE_MODEL_DATADOCUMENT_H

#include "../core/coreglobal.h"
#include "tablemodel.h"

#include <QObject>

namespace qf {
namespace core {
namespace model {

class QFCORE_DECL_EXPORT DataDocument : public QObject
{
	Q_OBJECT
	Q_ENUMS(RecordEditMode)
	Q_PROPERTY(qf::core::model::DataDocument::RecordEditMode mode READ mode WRITE setMode NOTIFY modeChanged)
	Q_PROPERTY(qf::core::model::TableModel* model READ model WRITE setModel NOTIFY modelChanged)
	Q_PROPERTY(QVariant dataId READ dataId WRITE setDataId NOTIFY dataIdChanged)
	Q_PROPERTY(QString idFieldName READ idFieldName WRITE setIdFieldName NOTIFY idFieldNameChanged)
public:
	explicit DataDocument(QObject *parent = 0);
	~DataDocument() Q_DECL_OVERRIDE;
public:
	enum RecordEditMode {ModeView, ModeEdit, ModeInsert, ModeCopy, ModeDelete};
	/*
	enum RecordEditMode {
		ModeView = TableModel::ModeView,
		ModeEdit = TableModel::ModeEdit,
		ModeInsert = TableModel::ModeInsert,
		ModeCopy = TableModel::ModeCopy,
		ModeDelete = TableModel::ModeDelete
	};
	*/

	QF_PROPERTY_IMPL2(QString, i, I, dFieldName, QStringLiteral("id"))
	QF_PROPERTY_IMPL(QVariant, d, D, ataId)
	QF_PROPERTY_IMPL2(RecordEditMode, m, M, ode, ModeView)
	QF_PROPERTY_IMPL2(int, c, C, urrentModelRow, 0)
public:
	void setModel(TableModel *m);
	virtual TableModel* model();
	Q_SIGNAL void modelChanged(TableModel *new_m);

	Q_SIGNAL void loaded();
	Q_SIGNAL void aboutToSave(const QVariant &id, int mode);
	Q_SIGNAL void saved(const QVariant &id, int mode);
	Q_SIGNAL void aboutToDrop(const QVariant &id);
	Q_SIGNAL void dropped(const QVariant &id);
	Q_SIGNAL void valueChanged(const QString &data_id, const QVariant &old_val, const QVariant &new_val);

	Q_SLOT void load(const QVariant &id, RecordEditMode mode = ModeEdit);
	Q_SLOT void loadForInsert() {load(QVariant(), ModeInsert);}
	//! this is a convinience function, @see load() .
	Q_SLOT virtual void load();
	Q_SLOT virtual void save();
	Q_SLOT virtual void drop();
	Q_SLOT virtual void copy();

	//virtual QVariant::Type fieldType(const QString &data_id);
	Q_INVOKABLE virtual bool isEmpty();
	Q_INVOKABLE QStringList fieldNames();
	Q_INVOKABLE virtual bool isValidFieldName(const QString &data_id);
	Q_INVOKABLE virtual bool isDirty(const QString &data_id);
	Q_INVOKABLE virtual QVariant value(const QString &data_id);
	Q_INVOKABLE QVariant value(const QString &data_id, const QVariant &default_value)
	{
		if(isValidFieldName(data_id) && !isEmpty())
			return value(data_id);
		return default_value;
	}
	Q_INVOKABLE virtual QVariant origValue(const QString &data_id);
	//Q_INVOKABLE QVariant origValue(const QString &data_id, const QVariant &default_value);
	Q_SLOT virtual void setValue(const QString &data_id, const QVariant &val);
	//Q_SLOT void setValue_qml(const QVariant &data_id, const QVariant &val) {setValue(data_id.toString(), val);}
protected:
	///! load model persistent storage via model
	Q_INVOKABLE virtual bool loadData();
	Q_INVOKABLE QVariant loadData_qml() {return loadData();}
	bool invokeLoadData() {return qf::core::Utils::invokeMethod_B_V(this, "loadData_qml");}

	///! save dirty data to persistent storage via model
	Q_INVOKABLE virtual bool saveData();
	Q_INVOKABLE QVariant saveData_qml() {return saveData();}
	bool invokeSaveData() {return qf::core::Utils::invokeMethod_B_V(this, "saveData_qml");}

	///! drop data in persistent storage via model
	Q_INVOKABLE virtual bool dropData();
	Q_INVOKABLE QVariant dropData_qml() {return dropData();}
	bool invokeDropData() {return qf::core::Utils::invokeMethod_B_V(this, "dropData_qml");}

	//! prepare loaded data to create document copy on next save() call
	Q_INVOKABLE virtual bool copyData();
	Q_INVOKABLE QVariant copyData_qml() {return copyData();}
	bool invokeCopyData() {return qf::core::Utils::invokeMethod_B_V(this, "copyData_qml");}


	virtual TableModel* createModel(QObject *parent);
protected:
	TableModel *m_model = nullptr;
};

}}}

#endif // QF_CORE_MODEL_DATADOCUMENT_H
