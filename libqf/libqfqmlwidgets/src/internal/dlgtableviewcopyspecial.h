#ifndef QF_QMLWIDGETS_INTERNAL_DLGTABLEVIEWCOPYSPECIAL_H
#define QF_QMLWIDGETS_INTERNAL_DLGTABLEVIEWCOPYSPECIAL_H

#include <QDialog>

#include "../tableview.h"

namespace qf {
namespace qmlwidgets {
namespace internal {

namespace Ui {class DlgTableViewCopySpecial;}

class DlgTableViewCopySpecial : public QDialog
{
	Q_OBJECT
private:
	typedef QDialog Super;
public:
	DlgTableViewCopySpecial(QWidget *parent = nullptr);
	~DlgTableViewCopySpecial() Q_DECL_OVERRIDE;
public:
	QString fieldsSeparator();
	QString rowsSeparator();
	QString fieldsQuotes();
	qf::qmlwidgets::TableView::ReplaceEscapes replaceEscapes();
private:
	Ui::DlgTableViewCopySpecial *ui;
};

}}}

#endif // QF_QMLWIDGETS_INTERNAL_DLGTABLEVIEWCOPYSPECIAL_H
