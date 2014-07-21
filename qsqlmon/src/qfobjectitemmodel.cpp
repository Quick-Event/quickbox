/********************************************************************
	created:	2005/01/02 14:27
	filename: 	QFObjectItemModel.cpp
	author:		Fanda Vacek (fanda.vacek@volny.cz)
*********************************************************************/

#include "qfobjectitemmodel.h"

#include <qf.h>
#include <qfassert.h>

#include <QIcon>
#include <QApplication>
#include <QStyle>

#include <qflogcust.h>
		
//=============================================
//                QFObjectItemModelRoot
//=============================================
QFObjectItemModelRoot::QFObjectItemModelRoot(QFObjectItemModel *_model)
	: QObject(_model)
{
}

QFObjectItemModel* QFObjectItemModelRoot::model()
{
	QFObjectItemModel *ret = qobject_cast<QFObjectItemModel*>(parent());
	QF_ASSERT(ret, "internal error");
	return ret;
}

//=============================================
//                QFObjectItemModel
//=============================================
QFObjectItemModel::QFObjectItemModel(QObject *parent) 
	: QAbstractItemModel(parent)
{
	rootObj = NULL;
	//deletedList = new QObject(this);
	init();
}
QFObjectItemModel::~QFObjectItemModel() 
{
	qfTrash() << QF_FUNC_NAME << "root object:" << rootObj;
	SAFE_DELETE(rootObj);
}

void QFObjectItemModel::init()
{
	SAFE_DELETE(rootObj);
	rootObj = new QFObjectItemModelRoot(this);
	rootObj->setObjectName("ObjectItemModelRoot");
}

void QFObjectItemModel::reset()
{
	beginResetModel();
	objectMap.clear();
	endResetModel();
}
/*
void ObjectItemModel::setRootObject(QObject *new_root)
{
	SAFE_DELETE(rootObj);
	rootObj = new ObjectItemModelRoot(this);
}
*/
QObject* QFObjectItemModel::index2object(const QModelIndex& ix) const
{
	void *p = ix.internalPointer();
	//qfTrash() << QF_FUNC_NAME << "object:" << ret;
	//if(ret) ret->dumpObjectInfo(); else qfTrash() << "NULL";
	QObject *ret = objectMap.value(p, NULL);
	Q_ASSERT_X(ret != rootObj, "QFObjectItemModel::index2object()", "indexMap contains rootObj.");
    return ret;    
}

QModelIndex QFObjectItemModel::object2index(int row, int column, QObject *o) const
{
    QModelIndex ret;
    if(!o) return ret;
    if(o == rootObj) return ret;

	//qfTrash() << QF_FUNC_NAME;
	//o->dumpObjectInfo();
	// insert object to object map
	void *v = static_cast<void*>(o);
	ObjectMap &m = const_cast<ObjectMap &>(objectMap); 
	m[v] = o;

    return createIndex(row, column, v);
}

QModelIndex QFObjectItemModel::object2index(QObject *o) const
{
	//qfTrash() << QF_FUNC_NAME;
	QModelIndex ret;
    if(!o) return ret;
    if(o == rootObj) return ret;
	// check if pointer is valid (not deleted during lifetime of index)
	//o = objectMap.value(o, NULL);
	//if(!o) return ret;
	// find node in its parent node children list
	//qfTrash() << QF_FUNC_NAME << "object:" << o;
	//o->dumpObjectInfo();
	//qfTrash() << "\tEND of object dump";
	QObject *p = o->parent();
	//qfTrash() << "\tEND of parent";
	QF_ASSERT(p, "index object should have parent object.");

	int row = -1;
	int r1 = 0;
	QObjectList lst = p->children();
	for(int i=0; i<lst.count(); i++) {
		QObject *oo = lst[i];
		if(!isObjectAccepted(oo)) continue;
		if(oo == o) {
			row = r1;
			break;
		}
		r1++;
	}
	if(row < 0) {
		qfError() << " index NOT FOUND";
		return ret;
	}
	
	Q_ASSERT_X(row >= 0, "QFObjectItemModel::node2index()", "index object is not a child of its own parent.");
	//qfTrash() << QF_FUNC_NAME << "*************OUT";
	return object2index(row, 0, o);
}

//---------------------------------------------
QModelIndex QFObjectItemModel::index(int row, int column, const QModelIndex &parent) const
{
	//qfTrash() << QF_FUNC_NAME;
    QModelIndex ret;
    if (row >= 0 && column >= 0) {
	    QObject *p = index2object(parent);
        //qDebug() << "\tOF: " << "row: "<<parent.row()<<" col: "<<parent.column()<<" data: "<<o;
		if(!p) p = rootObj;

		int r = 0;
		QObjectList lst = p->children();
		for(int i=0; i<lst.count(); i++) {
			QObject *o = lst[i];
			if(!isObjectAccepted(o)) continue;
			if(r == row) {
				ret = object2index(r, column, o);
				break;
			}
			r++;
		}
    }
    //qDebug() << "\tRETURN: " << "row: "<<ret.row()<<" col: "<<ret.column()<<" data: "<<o;
	//qDebug() << __PRETTY_FUNCTION__ << " ... out object:" << ret.internalPointer();
	//qfTrash() << QF_FUNC_NAME << "*************OUT";
	return ret;
}

//---------------------------------------------
QModelIndex QFObjectItemModel::parent(const QModelIndex &child) const
{
	//qfTrash() << QF_FUNC_NAME << " ... in child object:" << child.internalPointer();
    QModelIndex ret;
    do {
	    QObject *o = NULL;
        o = index2object(child);
		//qfTrash() << "\tparent of " << o;
		if(!o) break;
		//qfTrash() << "\t\tname" << o->objectName();
		if(o == rootObj) break;
		//o->dumpObjectInfo();
		QObject *p = o->parent();
		//qfTrash() << "\tis " << p;
		//if(p) qfTrash() << "\t\tname" << p->objectName();
		ret = object2index(p);
    } while(false);
    return ret;
}

//---------------------------------------------
int QFObjectItemModel::rowCount(const QModelIndex & parent) const  
{
	//qfLogFuncFrame();
	int ret = 0;
	QObject *o = index2object(parent);
	if(!o) o = rootObj;
	if(o) {
		//const char * class_name = o->metaObject()->className();
		//if(class_name == QString("KZGroupItemDoplnky")) return 0;
		//qfInfo() << "\t class_name:" << class_name;
		QObjectList lst = o->children();
		for(int i=0; i<lst.count(); i++) {
			QObject *oo = lst[i];
			if(!isObjectAccepted(oo)) continue;
			ret++;
		}
		//qfInfo() << "\tRETURN: " << ret;
	}
	return ret;
}

//---------------------------------------------
int QFObjectItemModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
    //qDebug("columnCount() called model: %p data: %p row: %i col: %i ret: %i", parent.model(), parent.data(), parent.row(), parent.column(), ret);
	return 2;
}

//---------------------------------------------
bool QFObjectItemModel::hasChildren(const QModelIndex & parent) const 
{
	return (rowCount(parent) > 0);
}

//---------------------------------------------
QVariant QFObjectItemModel::data(const QModelIndex& index, int role) const 
{
	Q_UNUSED(index);
	Q_UNUSED(role);
	QVariant ret;
	return ret;
}

QVariant QFObjectItemModel::headerData(int section, Qt::Orientation o, int role) const
{
	QVariant ret;
	if (o == Qt::Horizontal) {
        if(role == Qt::DisplayRole) switch(section) {
			case 0: ret = "Name"; break;
			case 1: ret = "Attributes"; break;
        }
    }
	if(ret.isNull()) ret = QAbstractItemModel::headerData(section, o, role);
	return ret;
}
		
void QFObjectItemModel::append(QObject *o, QModelIndex parent_ix) throw(QFException)
{
	qfLogFuncFrame();
	QObject *par_o = index2object(parent_ix);
	if(!par_o) par_o = rootObj;
	if(!par_o) QF_EXCEPTION("Root object is NULL.");
	o->setParent(NULL); /// pokud by totiz o jiz byl potomkem par_o, blbnul by mi rowCount() a treeView by ukazovalo nesmysly (od Qt 4.6)
	beginInsertRows(parent_ix, rowCount(parent_ix), rowCount(parent_ix));
	o->setParent(par_o);
	endInsertRows();
}

void QFObjectItemModel::append(QList<QObject*> olst, QModelIndex parent_ix) throw(QFException)
{
	qfLogFuncFrame();
	QObject *par_o = index2object(parent_ix);
	if(!par_o) par_o = rootObj;
	if(!par_o) QF_EXCEPTION("Root object is NULL.");
	if(olst.count()) {
		foreach(QObject *o, olst) o->setParent(NULL);
		beginInsertRows(parent_ix, rowCount(parent_ix), rowCount(parent_ix) + olst.count()-1);
		foreach(QObject *o, olst) o->setParent(par_o);
		endInsertRows();
	}
}

void QFObjectItemModel::append(QObject *o, QObject *parent_o) throw(QFException)
{
	qfLogFuncFrame();
	if(!parent_o) parent_o = rootObj;
	if(!parent_o) QF_EXCEPTION("Root object is NULL.");
	QModelIndex parent_ix = object2index(parent_o);
	qfTrash() << "\t parent index row:" << parent_ix.row() << "col:" << parent_ix.column() << "internal pointer:" << parent_ix.internalPointer();
	qfTrash() << "\t beginInsertRows() rowCount(parent_ix):" << rowCount(parent_ix);
	o->setParent(NULL); /// pokud by totiz o jiz byl potomkem par_o, blbnul by mi rowCount() a treeView by ukazovalo nesmysly (od Qt 4.6)
	beginInsertRows(parent_ix, rowCount(parent_ix), rowCount(parent_ix));
	qfTrash() << "\t appending object:" << o << o->metaObject()->className() << o->objectName() << " o->parent():" << parent_o << parent_o->metaObject()->className();
	o->setParent(parent_o);
	endInsertRows();
}

void QFObjectItemModel::append(QList<QObject*> olst, QObject *parent_o) throw(QFException)
{
	qfLogFuncFrame();
	if(!parent_o) parent_o = rootObj;
	if(!parent_o) QF_EXCEPTION("Root object is NULL.");
	QModelIndex parent_ix = object2index(parent_o);
	if(olst.count()) {
		foreach(QObject *o, olst) o->setParent(NULL);
		beginInsertRows(parent_ix, rowCount(parent_ix), rowCount(parent_ix) + olst.count()-1);
		foreach(QObject *o, olst) {
			qfTrash() << "\t appending object:" << o << o->metaObject()->className() << o->objectName() << " o->parent():" << parent_o << parent_o->metaObject()->className();
			o->setParent(parent_o);
		}
		endInsertRows();
	}
}

QObject* QFObjectItemModel::take(const QModelIndex &ix)
{
	qfLogFuncFrame();
	QObject *o = index2object(ix);
	if(o) {
		beginRemoveRows(parent(ix), ix.row(), ix.row());
		qfTrash() << "\t o:" << o << o->metaObject()->className() << o->objectName() << " o->parent():" << o->parent();
		//o->dumpObjectInfo();
		o->setParent(NULL);
		objectMap.take(o);
		endRemoveRows();
	}
	//qDebug() << "removeObject" << o;
	return o;
}
		
void QFObjectItemModel::deleteChildren(const QModelIndex &parent_ix)
{
	qfLogFuncFrame();
	//qfInfo() << QFLog::stackTrace();
	QObject *o = index2object(parent_ix);
	if(!o) o = rootObj;
	//if(!o) o->dumpObjectInfo(); else qfTrash() << "NULL";
	if(o) {
		int n2 = rowCount(parent_ix)-1;
		qfTrash() << "\tbeginRemoveRows():" << o->metaObject()->className() << o->objectName() << "from" << 0 << "to" << n2;
		if(n2 >= 0) {
			beginRemoveRows(parent_ix, 0, n2);
			foreach(o, o->children()) {
				qfTrash() << "\twant delete object:" << o << o->objectName();
				if(!isObjectAccepted(o)) continue;
				qfTrash() << "\tdeleting object:" << o << o->metaObject()->className() << o->objectName();
				objectMap.take(o);
				o->setParent(NULL);
				SAFE_DELETE(o);
			}
			qfTrash() << "\tendRemoveRows():";
			endRemoveRows();
		}
	}
}
/*
void QFObjectItemModel::emitRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
	emit rowsAboutToBeRemoved(parent, start, end);
}
void QFObjectItemModel::emitRowsInserted(const QModelIndex & parent, int start, int end)
{
	emit rowsInserted(parent, start, end);
}
*/
//=============================================

