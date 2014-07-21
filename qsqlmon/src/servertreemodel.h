#ifndef SERVERTREEMODEL_H
#define SERVERTREEMODEL_H

/********************************************************************
	created:	2005/01/02 14:27
	filename: 	ServerTreeModel.h
	author:		Fanda Vacek (fanda.vacek@volny.cz)
*********************************************************************/

//#include <qfexception.h>
#include <qfobjectitemmodel.h>
//#include <qfxmlconfigdocument.h>

#include <QWidget>

//=============================================
//                ServerTreeModel
//=============================================
/**
 * Data model for QObject tree and QTreeView.
 */ 
class ServerTreeModel : public QFObjectItemModel
{
	Q_OBJECT
protected:
	QVariantMap servers;
public:
	/**
	    Sets model contens to XML file.
		@return false if error occures
	*/
	//bool setContent(QFile &f) throw(exception);

	// tyhle musim implementovat pro ReadOnly
	/*
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent ( const QModelIndex & index ) const;
	int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
	*/
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QVariant headerData ( int section, Qt::Orientation o, int role = Qt::DisplayRole ) const;

public:
	void load(const QVariantMap& el);
public:
	ServerTreeModel(QObject *parent = NULL) : QFObjectItemModel(parent) { }
	~ServerTreeModel() {
		//qDebug("~ServerTreeModel() - indexedNodes.count(): %i", indexedNodes.count());
	}
};
//=============================================

#endif
