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

	void setPrinterOptions(const ReceiptsPrinterOptions &opts);
	ReceiptsPrinterOptions printerOptions();
private:
	void loadPrinters();
private:
	Ui::ReceiptsPrinterOptionsDialog *ui;
};

#endif // RECEIPTPRINTEROPTIONSDIALOG_H
