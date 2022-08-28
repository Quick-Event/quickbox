#ifndef RECEIPTPRINTEROPTIONSDIALOG_H
#define RECEIPTPRINTEROPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ReceiptsPrinterOptionsDialog;
}

class ReceiptsPrinterOptions;

class ReceiptsPrinterOptionsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit ReceiptsPrinterOptionsDialog(QWidget *parent = 0);
	~ReceiptsPrinterOptionsDialog();

	void accept();
private:
	void load();
	void save();
	void loadPrinters();
private:
	Ui::ReceiptsPrinterOptionsDialog *ui;
};

#endif // RECEIPTPRINTEROPTIONSDIALOG_H
