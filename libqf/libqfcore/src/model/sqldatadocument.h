#ifndef QF_CORE_MODEL_SQLDATADOCUMENT_H
#define QF_CORE_MODEL_SQLDATADOCUMENT_H

#include "datadocument.h"
#include "sqltablemodel.h"

namespace qf {
namespace core {
namespace model {

class QFCORE_DECL_EXPORT SqlDataDocument : public DataDocument
{
	Q_OBJECT
public:
	SqlDataDocument(QObject *parent = nullptr);
	~SqlDataDocument() Q_DECL_OVERRIDE;
private:
	typedef DataDocument Super;
public:
	virtual SqlTableModel* model();
protected:
	SqlTableModel* createModel(QObject *parent) Q_DECL_OVERRIDE;

	///! load model persistent storage via model
	virtual bool loadData();
	/*
	///! save dirty data to persistent storage via model
	virtual bool saveData();
	///! drop data in persistent storage via model
	virtual bool dropData();
	//! prepare loaded data to create document copy on next save() call
	virtual bool copyData();
	*/
};

}}}

#endif // QF_CORE_MODEL_SQLDATADOCUMENT_H
