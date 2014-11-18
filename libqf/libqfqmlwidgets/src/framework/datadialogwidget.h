#ifndef DATADIALOGWIDGET_H
#define DATADIALOGWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "../datacontroller.h"
#include "dialogwidget.h"

#include <qf/core/model/datadocument.h>

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT DataDialogWidget : public DialogWidget
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::DataController* dataController READ dataController WRITE setDataController)
private:
	typedef DialogWidget Super;
public:
	DataDialogWidget(QWidget *parent = nullptr);
	~DataDialogWidget() Q_DECL_OVERRIDE;
public:
	qf::qmlwidgets::DataController *dataController();
	void setDataController(qf::qmlwidgets::DataController *dc);

	Q_SLOT void load(const QVariant &id = QVariant(), qf::core::model::DataDocument::RecordEditMode mode = qf::core::model::DataDocument::ModeEdit);

	bool dialogDoneRequest(int result) Q_DECL_OVERRIDE;

	Q_SIGNAL void dataSaved(const QVariant &id, int mode);
protected:
	virtual bool saveData();
protected:
	qf::qmlwidgets::DataController *m_dataController = nullptr;
};

}}}

#endif // DATADIALOGWIDGET_H
