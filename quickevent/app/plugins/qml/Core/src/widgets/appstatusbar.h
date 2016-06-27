#ifndef APPSTATUSBAR_H
#define APPSTATUSBAR_H

#include <qf/core/utils.h>

#include <qf/qmlwidgets/statusbar.h>

class QLabel;
class QProgressBar;

class AppStatusBar : public qf::qmlwidgets::StatusBar
{
	Q_OBJECT

	Q_PROPERTY(QString eventName READ eventName WRITE setEventName NOTIFY eventNameChanged)
	Q_PROPERTY(int stageNo READ stageNo WRITE setStageNo NOTIFY stageNoChanged)
private:
	typedef qf::qmlwidgets::StatusBar Super;
public:
	AppStatusBar(QWidget *parent = nullptr);
	~AppStatusBar() Q_DECL_OVERRIDE;

	QString eventName() const {return m_eventName;}
	Q_SLOT void setEventName(const QString &event_name);
	Q_SIGNAL void eventNameChanged();

	int stageNo() const {return m_stageNo;}
	Q_SLOT void setStageNo(int stage_no);
	Q_SIGNAL void stageNoChanged();

	void showProgress(const QString &msg, int completed, int total) Q_DECL_OVERRIDE;
private:
	QLabel *m_lblMessage;
	QProgressBar *m_progress;
	QLabel *m_lblEvent;
	QLabel *m_lblStage;

	QString m_eventName;
	int m_stageNo = 0;
};

#endif // APPSTATUSBAR_H
