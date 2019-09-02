#ifndef SERVERTREEMODEL_H
#define SERVERTREEMODEL_H

/********************************************************************
	created:	2005/01/02 14:27
	filename: 	ServerTreeModel.h
	author:		Fanda Vacek (fanda.vacek@volny.cz)
*********************************************************************/

#include "qfobjectitemmodel.h"

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
private:
	typedef QFObjectItemModel Super;
public:
	ServerTreeModel(QObject *parent = nullptr);
	~ServerTreeModel() Q_DECL_OVERRIDE;
public:
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const Q_DECL_OVERRIDE;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
	QVariant headerData ( int section, Qt::Orientation o, int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;

public:
	void loadSettings();
	void saveSettings();
};
//=============================================

#endif
