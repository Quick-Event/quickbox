
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//

#include "qfstatusbar.h"

#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QLabel>
#include <QProgressBar>
#include <QBoxLayout>
#include <QApplication>

QFStatusBar::QFStatusBar(QWidget *parent)
	: QStatusBar(parent)
{
	//for(int i=0; i<label_cnt; i++) addLabel();

	fProgressBar = new QProgressBar(this);
	//fProgressBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	fProgressBar->setMinimum(0);
	fProgressBar->setMaximum(100);
	fProgressBar->setValue(0);
	addWidget(fProgressBar);

	fProgressBarLabel = new QLabel(this);
	//fProgressBarLabel->setFrameShape(QFrame::Box);
	fProgressBarLabel->hide();
	addWidget(fProgressBarLabel);
/*
	lbl = statusBarLabels[0];
	lbl = new QLabel(tr("not connected"));
    //lblSb1->setAlignment(Qt::AlignHCenter);
	lbl->setMinimumSize(lbl->sizeHint());
	statusBar()->addWidget(lbl);
	statusBarWidgets.append(lbl);
	lbl = new QLabel(tr("info"));
	statusBar()->addWidget(lbl);
	statusBarWidgets.append(lbl);
	*/
	hideProgress();
}

QFStatusBar::~QFStatusBar()
{
}

void QFStatusBar::hideProgress()
{
	progressBar()->setTextVisible(true);
	progressBar()->hide();
	fProgressBarLabel->hide();
	//qApp->processEvents();
}

void QFStatusBar::setProgressValue(double val, const QString &label_text)
{
	qfLogFuncFrame() << "val:" << val << "text:" << label_text;
	if(val < 0 || val > 1) hideProgress();
	else {
		progressBar()->setValue(progressBar()->minimum() + (int)((progressBar()->maximum() - progressBar()->minimum()) * val));
		if(progressBar()->isHidden()) {
			progressBar()->show();
			//fProgressBarLabel->show();
		}
	}
	if(label_text.isEmpty()) fProgressBarLabel->hide();
	else {
		fProgressBarLabel->show();
		fProgressBarLabel->setText(label_text);
	}
	QApplication::processEvents();
}

void QFStatusBar::addLabel()
{
	//QList<QLabel*> lst = findChildren<QLabel*>();
	//int ix = lst.count();
	QBoxLayout *ly = qobject_cast<QBoxLayout*>(layout());
	QF_ASSERT_EX(ly, "bad layout");
	//ly->setMargin(0);
	QLabel *lbl = new QLabel(QString());//::number(labels.count()));
	lbl->setFrameStyle(static_cast<int>(QFrame::StyledPanel) | static_cast<int>(QFrame::Plain));
	ly->insertWidget(labels.count(), lbl);
	labels << lbl;
	setProgressValue(-1);
}

QLabel* QFStatusBar::label(int ix)
{
	//qfLogFuncFrame();
	while(ix >= labels.count()) addLabel();
	return labels[ix];
}

void QFStatusBar::setLabelText(int label_no, const QString &text)
{
	QLabel *lbl = label(label_no);
	if(text.isEmpty()) lbl->hide();
	else {
		lbl->show();
		lbl->setText(text);
	}
}

QString QFStatusBar::text()
{
	return label(0)->text();
}
