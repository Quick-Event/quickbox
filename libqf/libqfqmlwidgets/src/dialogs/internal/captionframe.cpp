#include "captionframe.h"

#include <qf/core/log.h>

#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QStyle>

using namespace qf::qmlwidgets::dialogs::internal;

CaptionFrame::CaptionFrame(QWidget *parent)
	: QFrame(parent)
{
	qfLogFuncFrame();
	setFrameShape(QFrame::StyledPanel);
	setFrameShadow(QFrame::Raised);
	QBoxLayout *ly = new QHBoxLayout(this);
	ly->setMargin(0);
	//ly->setContentsMargins(5, 1, 5, 1);
	ly->setSpacing(6);
	captionIconLabel = new QLabel();
		//captionLabel->setPixmap(icon.pixmap(32));
	ly->addWidget(captionIconLabel);

	captionLabel = new QLabel();
	ly->addWidget(captionLabel);
	ly->addStretch();
	closeButton = new QToolButton();
	QStyle *sty = style();
	closeButton->setIcon(sty->standardIcon(QStyle::SP_DialogDiscardButton));
	connect(closeButton, SIGNAL(clicked()), this, SIGNAL(closeButtonClicked()));
	closeButton->setAutoRaise(true);
	ly->addWidget(closeButton);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	setFrameStyle(StyleDefault);
}

void CaptionFrame::setFrameStyle(CaptionFrame::FrameStyle st)
{
	const char *style_sheets[] = {
		"QFrame{background:rgb(245, 245, 184)} QLabel{font: 14pt;color:rgb(67, 67, 67)}",
		"QFrame{background:rgb(245, 245, 184)} QLabel{font: 14pt;color:rgb(67, 67, 67)}",
		"QFrame{background:rgb(67, 67, 67)} QLabel{font: 18pt;color:white}"
	};
	setStyleSheet(style_sheets[st]);
}

void CaptionFrame::setText(const QString & s)
{
	qfLogFuncFrame() << "text:" << s;
	qfDebug() << "\t label:" << captionLabel;
	captionLabel->setText(s);
	setVisible(!(captionLabel->text().isEmpty() && captionIconLabel->pixmap() == NULL));
}

QString CaptionFrame::text() const
{
	return captionLabel->text();
}

void CaptionFrame::setIcon(const QIcon & ico)
{
	captionIconLabel->setPixmap(ico.pixmap(32));
	setVisible(!(captionLabel->text().isEmpty() && captionIconLabel->pixmap() == NULL));
}

void CaptionFrame::setCloseButtonVisible(bool b)
{
	//qfInfo() << "set close button visible:" << b;
	closeButton->setVisible(b);
}

