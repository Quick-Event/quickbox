#ifndef QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H
#define QF_QMLWIDGETS_DIALOGS_DIALOG_INTERNAL_CAPTIONFRAME_H

#include <qf/core/utils.h>

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
	Q_PROPERTY(QString text READ text WRITE setText)
	Q_PROPERTY(bool alert READ isAlert WRITE setAlert NOTIFY alertChanged)
public:
	//enum FrameStyle {StyleDefault=0, StyleLight, StyleDark};
public:
	CaptionFrame(QWidget *parent = nullptr);

	QF_PROPERTY_BOOL_IMPL2(a, A, lert, false)
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
	Q_SLOT void setRecordEditMode(int mode);
protected:
	QString m_text;
	QString m_iconSource;
	QIcon m_icon;
	QLabel *m_captionIconLabel;
	QLabel *m_captionLabel;
	QToolButton *m_closeButton;
	int m_recordEditMode = -1;
};

}}}}

#endif
