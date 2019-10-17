#ifndef QFSTATUSBAR_H
#define QFSTATUSBAR_H

#include <QStatusBar>

class QLabel;
class QProgressBar;

class QFStatusBar : public QStatusBar
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText)
protected:
	QProgressBar *fProgressBar;
	QLabel *fProgressBarLabel;

	QList<QLabel*> labels;
protected:
	void addLabel();
public slots:
	//! Set progress bar value, if \a val is out of <0, 1> \a hideProgress() is called instead.
	void setProgressValue(double val, const  QString &label_text = QString());
	void setLabelText(int label_no, const QString &text);
public:
	//! resets and hides progress bar.
	void hideProgress();
	QProgressBar* progressBar() {return fProgressBar;}
	//! return i-th label from the left or Throw exception.
	QLabel* label(int i);

	void setText(const QString &txt) {setLabelText(0, txt);}
	QString text();
public:
	//! Create status bar with \a label_cnt labels from left and the progress bar.
	//! Minimal \a label_cnt is 1.
	QFStatusBar(QWidget *parent = nullptr);
	virtual ~QFStatusBar();
};

#endif // QFSTATUSBAR_H

