#ifndef QF_QMLWIDGETS_DIALOG_H
#define QF_QMLWIDGETS_DIALOG_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

#include <QDialog>
#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace qf {
namespace qmlwidgets {

class Frame;
class DialogButtonBox;

class QFQMLWIDGETS_DECL_EXPORT Dialog : public QDialog, public framework::IPersistentSettings, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(qf::qmlwidgets::DialogButtonBox* buttonBox READ buttonBox WRITE setButtonBox NOTIFY buttonBoxChanged)
	Q_PROPERTY(bool doneCancelled READ isDoneCancelled WRITE setDoneCancelled NOTIFY doneCancelledChanged)
	Q_ENUMS(DoneResult)
private:
	typedef QDialog Super;
public:
	enum DoneResult {ResultReject = Rejected, ResultAccept = Accepted, ResultDelete};
public:
	explicit Dialog(QWidget *parent = 0);
	~Dialog() Q_DECL_OVERRIDE;

	Q_SLOT void loadPersistentSettings(bool recursively = false);

	DialogButtonBox* buttonBox() {return m_dialogButtonBox;}
	void setButtonBox(DialogButtonBox *dbb);
	Q_SIGNAL void buttonBoxChanged();

	bool isDoneCancelled() {return m_doneCancelled;}
	void setDoneCancelled(bool b);
	Q_SIGNAL void doneCancelledChanged(bool new_val);
	Q_SIGNAL void aboutToBeDone(int result);

	void done(int result) Q_DECL_OVERRIDE;
protected:
	QQmlListProperty<QWidget> widgets();
private:
	Q_SLOT void savePersistentSettings();
	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
protected:
	Frame *m_centralFrame;
	DialogButtonBox *m_dialogButtonBox;
	bool m_doneCancelled;
};

}}

#endif // QF_QMLWIDGETS_DIALOG_H
