
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H


#include <QDialog>

namespace Ui {class DlgSettings;};

class CardReaderWidget;

//! TODO: write class documentation.
class  DlgSettings : public QDialog
{
	Q_OBJECT
private:
	Ui::DlgSettings *ui;
protected:
	void load();
	void save();
public:
	virtual void accept();
public:
	DlgSettings(QWidget *parent = NULL);
	virtual ~DlgSettings();
private:
};

#endif // DLGSETTINGS_H

