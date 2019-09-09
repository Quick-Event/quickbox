#ifndef QFOBJECTITEMMODEL_H
#define QFOBJECTITEMMODEL_H

/********************************************************************
	created:	2005/01/02 14:27
	filename: 	QFObjectItemModel.h
	author:		Fanda Vacek (fanda.vacek@volny.cz)
*********************************************************************/

#include <QMap>
#include <QAbstractItemModel>

//class QFObjectItemModel;

//=============================================
//                QFObjectItemModel
//=============================================
class QFObjectItemModel;
	
class QFObjectItemModelRoot : public QObject
{
	Q_OBJECT
	public:
		QFObjectItemModel *model();
	public:
		QFObjectItemModelRoot(QFObjectItemModel *_model);
};

/**
 * Data model for QObject tree and QTreeView.
 * Pointer in this model's QModelIndex points to indexed object.
 */
class QFObjectItemModel : public QAbstractItemModel
{
	Q_OBJECT
		
	public:
	protected:
		typedef QMap<void*, QObject*> ObjectMap;
		//! tohle tu musi bejt, protoze mi v QModelIndexech zustavaj pointery
		ObjectMap objectMap;
	public:
		void init();
	
		// tyhle musim implementovat pro ReadOnly
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent ( const QModelIndex & index ) const;
		int rowCount( const QModelIndex & parent = QModelIndex()) const;
		int columnCount ( const QModelIndex & parent = QModelIndex()) const;
		bool hasChildren ( const QModelIndex & parent = QModelIndex()) const;
		QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
		QVariant headerData ( int section, Qt::Orientation o, int role = Qt::DisplayRole ) const;
	
		// emits signal rowsAboutToBeRemoved()
		//void emitRowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
		// emits signal rowsInserted()
		//void emitRowsInserted(const QModelIndex & parent, int start, int end);
	protected:
		QFObjectItemModelRoot *m_rootObj;
	
		//! If this function returns false, object is simply ignored by model. Thats mean it is not dislayed in treeview.
		virtual bool isObjectAccepted(QObject *o) const {Q_UNUSED(o); return true;}
		void reset();
	public:
		/*
		// jeho detmi se stavaji vymazane objekty, je to potreba jednou za cas promazat :))
		// objekty nemuzu mazat rovnou, protoze muzou bejt jeste odkazovany z modelindexu
		QList<QObject*> deletedList;
		void deleteDeletedList() {
			foreach(QObject *o, deletedList) delete o;
		}
		*/
	protected:
		QModelIndex object2index(int row, int column, QObject *o) const;
	public:
		/// @return underlaing object or NULL
		QObject* index2object(const QModelIndex& ix) const;
		QModelIndex object2index(QObject *o) const;
	
	public:
		// insert object before \a row
		/*
		* \param row is currently ignored, because I do not know how to get non const
		* 			reference to the QObject children list.
		*/
		//void insertBefore(QObject *o, QModelIndex parent_ix, int row);
		//void insertAfter(QObject *o, QModelIndex parent_ix, int row);
		void append(QObject *o, QModelIndex parent_ix);
		void append(QList<QObject*> olst, QModelIndex parent_ix);
		void append(QObject *o, QObject *parent_o);
		void append(QList<QObject*> olst, QObject *parent_o);
		/**
		 *  unlink object from the models objects tree
		 *  @return pointer to the unlinked object
	 	 */
		QObject* take(const QModelIndex &ix);
		/// deleteChildren(QModelIndex()) vymaze vsechny deti.
		void deleteChildren(const QModelIndex &parent_ix = QModelIndex());

		void dump() {m_rootObj->dumpObjectTree();}
	public:
	/*
	void setRootObject(QObject *new_root);
	QObject* getRootObject(bool throw_exc = true)
	{
		if(!rootObj && throw_exc) throw FException("root is NULL");
		return rootObj;
	}
	*/
	public:
		QFObjectItemModel(QObject *parent = nullptr);
		~QFObjectItemModel();
};
//=============================================

//typedef QFLib::ObjectItemModel QFObjectItemModel;

#endif
