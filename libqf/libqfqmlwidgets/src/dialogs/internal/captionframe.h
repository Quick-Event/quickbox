#ifndef QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H
#define QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H

#include <QFrame>

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
	enum FrameStyle {StyleDefault=0, StyleLight, StyleDark};
public:
	CaptionFrame(QWidget *parent = NULL);
protected:
	QLabel *captionIconLabel;
	QLabel *captionLabel;
	QToolButton *closeButton;
signals:
	void closeButtonClicked();
public:
	void setText(const QString &s);
	QString text() const;
	void setIcon(const QIcon &ico);
	void setCloseButtonVisible(bool b = true);
	void setFrameStyle(FrameStyle st);
};

}}}}

#endif
