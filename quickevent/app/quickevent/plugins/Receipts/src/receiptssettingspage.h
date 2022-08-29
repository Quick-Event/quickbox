#pragma once

#include "../../Core/src/widgets/settingspage.h"

namespace Receipts {

namespace Ui {class ReceiptsSettingsPage;};

class  ReceiptsSettingsPage : public Core::SettingsPage
{
	Q_OBJECT

	using Super = Core::SettingsPage;
public:
	ReceiptsSettingsPage(QWidget *parent = nullptr);
	virtual ~ReceiptsSettingsPage();
protected:
	void load();
	void save();
private:
	void loadReceptList();
	void updateReceiptsPrinterLabel();
	void onPrinterOptionsClicked();
private:
	Ui::ReceiptsSettingsPage *ui;
};

}

