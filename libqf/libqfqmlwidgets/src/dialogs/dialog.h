#ifndef QF_QMLWIDGETS_DIALOGS_DIALOG_H
#define QF_QMLWIDGETS_DIALOGS_DIALOG_H

#include "../qmlwidgetsglobal.h"
#include "../framework/ipersistentsettings.h"

#include <QDialog>
#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT Dialog : public QDialog, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(bool doneCancelled READ isDoneCancelled WRITE setDoneCancelled NOTIFY doneCancelledChanged)
	Q_ENUMS(DoneResult)
private:
	typedef QDialog Super;
public:
	enum DoneResult {ResultReject = Rejected, ResultAccept = Accepted, ResultDelete};
public:
	explicit Dialog(QWidget *parent = 0);
	~Dialog() Q_DECL_OVERRIDE;
public:
	Q_SLOT void loadPersistentSettings();

	bool isDoneCancelled() {return m_doneCancelled;}
	void setDoneCancelled(bool b);
	Q_SIGNAL void doneCancelledChanged(bool new_val);
	Q_SIGNAL void aboutToBeDone(int result);

	void done(int result) Q_DECL_OVERRIDE;
private:
	Q_SLOT void savePersistentSettings();
protected:
	bool m_doneCancelled;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_DIALOG_H
