#ifndef CHOOSEORISEVENTDIALOG_H
#define CHOOSEORISEVENTDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseOrisEventDialog;
}

class OrisImporter;
class QCompleter;

class ChooseOrisEventDialog : public QDialog
{
	Q_OBJECT
private:
	using Super = QDialog;
public:
	explicit ChooseOrisEventDialog(OrisImporter *importer, QWidget *parent = 0);
	~ChooseOrisEventDialog();

	int exec() Q_DECL_OVERRIDE;
	int eventId();
private slots:
	void on_cbxOrisEvent_currentIndexChanged(int ix);

private:
	void load();
private:
	Ui::ChooseOrisEventDialog *ui;
	OrisImporter *m_importer;
	QCompleter *m_completer = nullptr;
};

#endif // CHOOSEORISEVENTDIALOG_H
