#ifndef QF_QMLWIDGETS_DIALOGS_GETITEMINPUTDIALOG_H
#define QF_QMLWIDGETS_DIALOGS_GETITEMINPUTDIALOG_H

#include "../qmlwidgetsglobal.h"

#include <QDialog>

class QLabel;
class QComboBox;

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT GetItemInputDialog : public QDialog
{
	Q_OBJECT
public:
	explicit GetItemInputDialog(QWidget *parent = 0);
	~GetItemInputDialog() Q_DECL_OVERRIDE;
public:
	void setLabelText(const QString &text);
	void setItems(const QStringList &items);
	int currentItemIndex();
	void setCurrentItemIndex(int ix);

	static int getItem(QWidget *parent, const QString &title, const QString &label_text, const QStringList &items, int current_item_index = -1);
private:
	QLabel *m_label;
	QComboBox *m_comboBox;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_GETITEMINPUTDIALOG_H
