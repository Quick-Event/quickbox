
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef CARDREADERSETTINGSPAGE_H
#define CARDREADERSETTINGSPAGE_H

#include "../../Core/src/widgets/settingspage.h"

namespace CardReader {

namespace Ui {class CardReaderSettingsPage;}

//class CardReaderWidget;

class  CardReaderSettingsPage : public Core::SettingsPage
{
	Q_OBJECT

	using Super = Core::SettingsPage;
private:
	Ui::CardReaderSettingsPage *ui;
protected:
	void load();
	void save();
public:
	CardReaderSettingsPage(QWidget *parent = nullptr);
	virtual ~CardReaderSettingsPage();
private slots:
	void on_btTestConnection_clicked();
};
}
#endif // CARDREADERSETTINGSPAGE_H

