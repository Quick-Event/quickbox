#include "getiteminputdialog.h"

#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>

using namespace qf::qmlwidgets::dialogs;

GetItemInputDialog::GetItemInputDialog(QWidget *parent) :
	QDialog(parent)
{
	m_label = new QLabel(QString(), this);
	m_comboBox = new QComboBox(this);
	m_label->setBuddy(m_comboBox);
	m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	QObject::connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
	QObject::connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));

	QBoxLayout *ly = new QVBoxLayout(this);
	//we want to let the input dialog grow to available size on Symbian.
	//ly->setSizeConstraint(QLayout::SetMinAndMaxSize);
	ly->addWidget(m_label);
	ly->addWidget(m_comboBox);
	ly->addWidget(button_box);
}

GetItemInputDialog::~GetItemInputDialog()
{
}

void GetItemInputDialog::setLabelText(const QString &text)
{
	m_label->setText(text);
}

void GetItemInputDialog::setItems(const QStringList &items, const QVariantList &data)
{
	m_comboBox->clear();
	for (int i = 0; i < items.count(); ++i) {
		m_comboBox->addItem(items[i], data.value(i));
	}
}

int GetItemInputDialog::currentItemIndex()
{
	return m_comboBox->currentIndex();
}

QString GetItemInputDialog::currentText()
{
	return m_comboBox->currentText();
}

QVariant GetItemInputDialog::currentData()
{
	return m_comboBox->currentData();
}

void GetItemInputDialog::setCurrentItemIndex(int ix)
{
	m_comboBox->setCurrentIndex(ix);
}

int GetItemInputDialog::getItem(QWidget *parent, const QString &title, const QString &label_text, const QStringList &items, int current_item_index)
{
	GetItemInputDialog dlg(parent);
	dlg.setWindowTitle(title);
	dlg.setLabelText(label_text);
	dlg.setItems(items);
	dlg.setCurrentItemIndex(current_item_index);
	if(dlg.exec())
		return dlg.currentItemIndex();
	return -1;
}
