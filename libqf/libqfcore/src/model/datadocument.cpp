#include "datadocument.h"

#include "../core/log.h"
#include "../utils/table.h"

namespace qfc = qf::core;
namespace qfu = qf::core::utils;
using namespace qf::core::model;

DataDocument::DataDocument(QObject *parent) : QObject(parent)
{

}

DataDocument::~DataDocument()
{

}

QString DataDocument::recordEditModeToString(DataDocument::RecordEditMode m)
{
	switch(m) {
	case ModeView: return QStringLiteral("ModeView");
	case ModeEdit: return QStringLiteral("ModeEdit");
	case ModeInsert: return QStringLiteral("ModeInsert");
	case ModeCopy: return QStringLiteral("ModeCopy");
	case ModeDelete: return QStringLiteral("ModeDelete");
	}
	return QStringLiteral("Invalid");
}

TableModel *DataDocument::createModel(QObject *parent)
{
	return new TableModel(parent);
}

void DataDocument::setModel(TableModel *m)
{
	if(m != m_model) {
		m_model = m;
		emit modelChanged(m);
	}
}

TableModel *DataDocument::model()
{
	if(!m_model) {
		m_model = createModel(this);
	}
	return m_model;
}

void DataDocument::load(const QVariant &id, DataDocument::RecordEditMode _mode)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << recordEditModeToString(_mode) << "isNull:" << id.isNull();
	setDataId(id);
	setMode(_mode);
	load();
}

void DataDocument::load()
{
	qfLogFuncFrame();
	RecordEditMode m = mode();
	QVariant id = dataId();
	if(m == ModeCopy && !id.isNull()) {
		if(invokeLoadData()) {
			copy();
		}
	}
	else {
		setMode(m);
		if(invokeLoadData()) {
			emit loaded();
		}
	}
}

void DataDocument::save()
{
	qfLogFuncFrame();
	if(mode() != ModeView) {
		RecordEditMode m = mode();
		QVariant id = dataId();
		emit aboutToSave(id, m);
		if(invokeSaveData()) {
			// reload data id, can be set from serie for mode INSERT
			id = dataId();
			qfDebug() << "emitting saved id:" << id.toString() << "mode:" << m;
			emit saved(id, m);
		}
	}
}

bool DataDocument::drop()
{
	qfLogFuncFrame();
	QVariant id = dataId();
	emit aboutToDrop(id);
	if(invokeDropData()) {
		emit dropped(id);\
		return true;
	}
	return false;
}

void DataDocument::copy()
{
	qfLogFuncFrame();
	if(mode() != ModeInsert) {
		invokeCopyData();
		setMode(ModeInsert);
		//setDataDirty_helper(isDataDirty());
		emit loaded();
	}
}

bool DataDocument::isEmpty()
{
	if(!m_model)
		return true;
	int ri = currentModelRow();
	bool ret = !(ri >= 0 && ri < model()->rowCount());
	return ret;
}

QStringList DataDocument::fieldNames()
{
	QStringList ret;
	TableModel *m = model();
	for(int i=0; i<m->columnCount(); i++)
		ret << m->columnDefinition(i).fieldName();
	return ret;
}

bool DataDocument::isValidFieldName(const QString &data_id)
{
	TableModel *m = model();
	return m->columnIndex(data_id) >= 0;
}

bool DataDocument::isDirty(const QString &data_id)
{
	bool ret = false;
	TableModel *m = model();
	int r = currentModelRow();
	ret = m->isDirty(r, data_id);
	return ret;
}

QVariant DataDocument::value(const QString &data_id)
{
	QVariant ret;
	TableModel *m = model();
	int r = currentModelRow();
	ret = m->value(r, data_id);
	return ret;
}

QVariant DataDocument::origValue(const QString &data_id)
{
	QVariant ret;
	TableModel *m = model();
	int r = currentModelRow();
	ret = m->origValue(r, data_id);
	return ret;
}

void DataDocument::setValue(const QString &data_id, const QVariant &val)
{
	qfLogFuncFrame() << "data_id:" << data_id << "val:" << val.toString();
	if(isEmpty()) {
		qfWarning() << "data_id:" << data_id << "val:" << val.toString() << "setValue() in empty document";
		return;
	}
	QVariant old_val;
	TableModel *m = model();
	int r = currentModelRow();
	old_val = m->value(r, data_id);
	m->setValue(r, data_id, val);
	if(old_val != val) {
		emit valueChanged(data_id, old_val, val);
	}
}

bool DataDocument::loadData()
{
	qfLogFuncFrame();
	if(mode() == ModeInsert) {
		int ri = currentModelRow();
		if(ri < 0 || ri >= model()->rowCount()) {
			model()->appendRow();
			setCurrentModelRow(model()->rowCount() - 1);
		}
	}
	else {
		if(currentModelRow() < 0)
			setCurrentModelRow(0);
	}
	return true;
}

bool DataDocument::saveData()
{
	qfLogFuncFrame();
	if(mode() != ModeView) {
		//qfTrash() << "\ttable type:" << model()->table()->metaObject()->className();
		QVariant old_id = dataId();
		model()->postRow(currentModelRow(), qf::core::Exception::Throw);
		if(mode() == ModeInsert) {
			QVariant id = value(idFieldName());
			qfDebug() << "\t id field name:" << idFieldName() << "inserted ID:" << id.toString();
			setDataId(id);
			emit valueChanged(idFieldName(), old_id, id);/// to refresh also widgets with dataId() == "id"
		}
	}
	if(mode() == ModeInsert) {
		setMode(ModeEdit);
	}
	return true;
}

bool DataDocument::dropData()
{
	qfLogFuncFrame();
	bool ret = true;
	if(mode() != ModeView) {
		ret = model()->dropRow(currentModelRow(), !qf::core::Exception::Throw);
	}
	return ret;
}

bool DataDocument::copyData()
{
	qfLogFuncFrame();
	if(isEmpty())
		return false;
	int ix = currentModelRow();
	return model()->prepareForCopyRow(ix);
}

