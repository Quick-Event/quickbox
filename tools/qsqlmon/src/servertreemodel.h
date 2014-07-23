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
	QVariantList m_connections;
public:
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	QVariant headerData ( int section, Qt::Orientation o, int role = Qt::DisplayRole ) const;

public:
	void load(const QVariantMap& params);
public:
	ServerTreeModel(QObject *parent = NULL) : QFObjectItemModel(parent) { }
	~ServerTreeModel() Q_DECL_OVERRIDE {}
};
//=============================================

#endif
