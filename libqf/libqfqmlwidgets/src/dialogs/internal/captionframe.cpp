#include "captionframe.h"
#include "../../framework/mainwindow.h"
#include "../../framework/plugin.h"
#include "../../style.h"

#include <qf/core/log.h>
#include <qf/core/model/datadocument.h>

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
	//ly->setMargin(0);
	ly->setContentsMargins(5, 1, 5, 1);
	ly->setSpacing(6);
	m_captionIconLabel = new QLabel();
		//captionLabel->setPixmap(icon.pixmap(32));
	ly->addWidget(m_captionIconLabel);

	m_captionLabel = new QLabel();
	ly->addWidget(m_captionLabel);
	ly->addStretch();
	m_closeButton = new QToolButton();
	m_closeButton->setVisible(false);
	QStyle *sty = style();
	m_closeButton->setIcon(sty->standardIcon(QStyle::SP_DialogDiscardButton));
	connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(closeButtonClicked()));
	m_closeButton->setAutoRaise(true);
	ly->addWidget(m_closeButton);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	//setFrameStyle(StyleDefault);
	connect(this, &CaptionFrame::alertChanged, this, &CaptionFrame::update);
}
/*
void CaptionFrame::setFrameStyle(CaptionFrame::FrameStyle st)
{
	const char *style_sheets[] = {
		"QFrame{background:rgb(67, 67, 67)} QLabel{font: 18pt;color:white}",
		"QFrame{background:rgb(245, 245, 184)} QLabel{font: 14pt;color:rgb(67, 67, 67)}",
		"QFrame{background:rgb(67, 67, 67)} QLabel{font: 18pt;color:white}"
	};
	setStyleSheet(style_sheets[st]);
}
*/
void CaptionFrame::setText(const QString & s)
{
	qfLogFuncFrame() << "text:" << s;
	qfDebug() << "\t label:" << m_captionLabel;
	m_text = s;
	m_text.replace('&', QString()); // remove accel keys mark, like &Run
	update();
}

QString CaptionFrame::text() const
{
	return m_text;
}

void CaptionFrame::setIcon(const QIcon & ico)
{
	m_icon = ico;
	update();
}

void CaptionFrame::setCloseButtonVisible(bool b)
{
	//qfInfo() << "set close button visible:" << b;
	m_closeButton->setVisible(b);
}

QIcon CaptionFrame::createIcon()
{
	QIcon ico;
	QString icon_path = m_iconSource;
	if(!icon_path.isEmpty()) {
		if(!icon_path.startsWith(QLatin1String(":/")) && !icon_path.startsWith('/')) {
			qf::qmlwidgets::framework::Plugin *plugin = qf::qmlwidgets::framework::MainWindow::frameWork()->pluginForObject(this);
			if(!plugin) {
				qfWarning() << "Cannot found plugin for:" << this << ", default icon will be set.";
			}
			else {
				icon_path = plugin->manifest()->homeDir() + "/" + icon_path;
				QPixmap pm(icon_path);
				if(pm.isNull())
					qfWarning() << "Cannot load icon on path:" << icon_path;
				else
					ico = QIcon(pm);
			}
		}
		if(ico.isNull())
			ico = QIcon(":/qf/qmlwidgets/images/under-construction.png");
	}
	return ico;
}

void CaptionFrame::setRecordEditMode(int mode)
{
	qfLogFuncFrame() << "mode:" << qf::core::model::DataDocument::recordEditModeToString(qf::core::model::DataDocument::RecordEditMode(mode));
	m_recordEditMode = mode;
	update();
}

void CaptionFrame::update()
{
	qfLogFuncFrame() << "text:" << text();
	QString label_text = text();
	if(m_recordEditMode >= 0) {
		qf::core::model::DataDocument::RecordEditMode mode = qf::core::model::DataDocument::RecordEditMode(m_recordEditMode);
		if(mode == qf::core::model::DataDocument::ModeDelete) {
			label_text = tr("Delete ") + label_text;
			setAlert(true);
		}
		else if(mode == qf::core::model::DataDocument::ModeEdit) {
			label_text = tr("Edit ") + label_text;
		}
	}
	bool is_visible = (!label_text.isEmpty() || !m_icon.isNull() || isAlert());
	setVisible(is_visible);
	if(is_visible) {
		if(isAlert()) {
			QPixmap pm = qf::qmlwidgets::Style::instance()->pixmap(":/qf/qmlwidgets/images/alert", 32);
			m_captionIconLabel->setPixmap(pm);
		}
		else {
			m_captionIconLabel->setPixmap(m_icon.pixmap(32));
		}
		m_captionLabel->setText(" " + label_text);
	}
}

