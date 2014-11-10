#ifndef QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H
#define QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H

#include <QFrame>
#include <QIcon>

class QLabel;
class QToolButton;

namespace qf {
namespace qmlwidgets {
namespace dialogs {
namespace internal {

class CaptionFrame : public QFrame
{
	Q_OBJECT
public:
	//enum FrameStyle {StyleDefault=0, StyleLight, StyleDark};
public:
	CaptionFrame(QWidget *parent = NULL);
signals:
	void closeButtonClicked();
public:
	void setText(const QString &s);
	QString text() const;
	void setIconSource(const QString &path) {m_iconSource = path;}
	void setIcon(const QIcon &ico);
	void setCloseButtonVisible(bool b = true);
	//void setFrameStyle(FrameStyle st);
	QIcon createIcon();
	Q_SLOT void update();
protected:
	QString m_iconSource;
	QIcon m_icon;
	QLabel *captionIconLabel;
	QLabel *captionLabel;
	QToolButton *closeButton;
};

}}}}

#endif
