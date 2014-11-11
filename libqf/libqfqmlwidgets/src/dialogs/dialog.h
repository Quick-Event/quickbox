#ifndef QF_QMLWIDGETS_DIALOGS_DIALOG_H
#define QF_QMLWIDGETS_DIALOGS_DIALOG_H

#include "../qmlwidgetsglobal.h"
#include "../framework/ipersistentsettings.h"

#include <QDialog>
#include <QQmlListProperty>
#include <QQmlParserStatus>

class QLabel;
class QToolButton;

namespace qf {
namespace qmlwidgets {

class DialogButtonBox;
class MenuBar;
class ToolBar;

namespace framework {
class DialogWidget;
}

namespace dialogs {

namespace internal {
class CaptionFrame;
}

class QFQMLWIDGETS_DECL_EXPORT Dialog : public QDialog, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_ENUMS(DoneResult)
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
	Q_PROPERTY(bool doneCancelled READ isDoneCancelled WRITE setDoneCancelled NOTIFY doneCancelledChanged)
	Q_PROPERTY(qf::qmlwidgets::DialogButtonBox* buttonBox READ buttonBox WRITE setButtonBox NOTIFY buttonBoxChanged)
private:
	typedef QDialog Super;
public:
	enum DoneResult {ResultReject = Rejected, ResultAccept = Accepted, ResultDelete};
public:
	explicit Dialog(QWidget *parent = 0);
	~Dialog() Q_DECL_OVERRIDE;
public:
	void setCentralWidget(QWidget *central_widget);

	Q_INVOKABLE MenuBar* menuBar();
	Q_INVOKABLE ToolBar* addToolBar();

	DialogButtonBox* buttonBox() {return m_dialogButtonBox;}
	void setButtonBox(DialogButtonBox *dbb);
	Q_SIGNAL void buttonBoxChanged();

	Q_SLOT void loadPersistentSettings();

	bool isDoneCancelled() {return m_doneCancelled;}
	void setDoneCancelled(bool b);
	Q_SIGNAL void doneCancelledChanged(bool new_val);
	Q_SIGNAL void aboutToBeDone(int result);

	void done(int result) Q_DECL_OVERRIDE;
private:
	Q_SLOT void savePersistentSettings();

	Q_SLOT void settleDownDialogWidget();
protected:
	void updateCaptionFrame();
	void updateLayout();
protected:
	bool m_doneCancelled;
	internal::CaptionFrame *m_captionFrame = nullptr;
	MenuBar *m_menuBar = nullptr;
	QList<ToolBar*> m_toolBars;
	QWidget *m_centralWidget = nullptr;
	DialogButtonBox *m_dialogButtonBox = nullptr;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_DIALOG_H
