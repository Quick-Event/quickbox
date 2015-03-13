#include "dialog.h"
#include "internal/captionframe.h"
#include "../frame.h"
#include "../framework/dialogwidget.h"
#include "../menubar.h"
#include "../toolbar.h"
#include "../dialogbuttonbox.h"
#include "../internal/desktoputils.h"

#include <qf/core/log.h>

#include <QVBoxLayout>
#include <QSettings>
#include <QShowEvent>

using namespace qf::qmlwidgets::dialogs;

Dialog::Dialog(QWidget *parent) :
	QDialog(parent), framework::IPersistentSettings(this)
{
	qfLogFuncFrame();
}

Dialog::~Dialog()
{
	savePersistentSettings();
}

void Dialog::setCentralWidget(QWidget *central_widget)
{
	if(central_widget != m_centralWidget) {
		QF_SAFE_DELETE(m_centralWidget);
		qf::qmlwidgets::framework::DialogWidget *dialog_widget = qobject_cast<qf::qmlwidgets::framework::DialogWidget *>(central_widget);
		m_centralWidget = central_widget;
		if(m_centralWidget) {
			m_centralWidget->setParent(nullptr);
			m_centralWidget->setParent(this);
		}
		if(dialog_widget) {
			QMetaObject::invokeMethod(this, "settleDownDialogWidget", Qt::QueuedConnection);
		}
	}
}

void Dialog::settleDownDialogWidget()
{
	qf::qmlwidgets::framework::DialogWidget *dialog_widget = qobject_cast<qf::qmlwidgets::framework::DialogWidget *>(m_centralWidget);
	if(dialog_widget) {
		QVariant dlg = QVariant::fromValue(this);
		bool ok = QMetaObject::invokeMethod(dialog_widget, "settleDownInDialog_qml", Qt::DirectConnection, Q_ARG(QVariant, dlg));
		if(!ok) {
			qfWarning() << this << "Method settleDownInDialog_qml() invocation failed!";
		}
		else {
			updateCaptionFrame();
			updateLayout();
		}
	}
}

qf::qmlwidgets::MenuBar* Dialog::menuBar()
{
	if(!m_menuBar) {
		m_menuBar = new MenuBar(this);
	}
	return m_menuBar;
}

qf::qmlwidgets::ToolBar *Dialog::addToolBar()
{
	qf::qmlwidgets::ToolBar *ret = new qf::qmlwidgets::ToolBar(this);
	m_toolBars << ret;
	return ret;
}

int Dialog::exec()
{
	loadPersistentSettingsRecursively();
	return Super::exec();
}

void Dialog::done(int result)
{
	qfLogFuncFrame() << result;
	QVariant ok = true;
	QMetaObject::invokeMethod(this, "doneRequest_qml", Qt::DirectConnection,
							  Q_RETURN_ARG(QVariant, ok),
							  Q_ARG(QVariant, result));
	if(ok.toBool()) {
		Super::done(result);
	}
}

void Dialog::loadPersistentSettings()
{
	qfLogFuncFrame();
	QString id = persistentSettingsId();
	if(id.isEmpty() && dialogWidget() && !dialogWidget()->persistentSettingsId().isEmpty()) {
		setPersistentSettingsId(dialogWidget()->persistentSettingsId() + "Dlg");
	}
	QString path = persistentSettingsPath();
	qfDebug() << "\t persistentSettingsPath:" << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		QRect geometry = settings.value("geometry").toRect();
		if(geometry.isValid()) {
			if(isSavePersistentPosition()) {
				geometry = qf::qmlwidgets::internal::DesktopUtils::moveRectToVisibleDesktopScreen(geometry);
				this->setGeometry(geometry);
			}
			else {
				this->resize(geometry.size());
			}
		}
	}
}

bool Dialog::doneRequest(int result)
{
	qfLogFuncFrame() << "result:" << result;
	QVariant ret = true;
	qf::qmlwidgets::framework::DialogWidget *dw = dialogWidget();
	if(dw) {
		QMetaObject::invokeMethod(dw, "dialogDoneRequest_qml", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, ret),
								  Q_ARG(QVariant, result));
	}
	return ret.toBool();
}

QVariant Dialog::doneRequest_qml(const QVariant &result)
{
	return doneRequest(result.toBool());
}

void Dialog::savePersistentSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		QRect geometry = this->geometry();
		QSettings settings;
		settings.beginGroup(path);
		settings.setValue("geometry", geometry);
	}
}

void Dialog::updateLayout()
{
	QLayout *ly_orig = layout();
	QF_SAFE_DELETE(ly_orig);
	QBoxLayout *ly_root = new QVBoxLayout();
	ly_root->setMargin(0);
	ly_root->setSpacing(0);
	setLayout(ly_root);

	if(m_menuBar)
		ly_root->addWidget(m_menuBar);

	if(m_captionFrame)
		ly_root->addWidget(m_captionFrame);

	if(m_toolBars.count() == 1) {
		ToolBar *tb = m_toolBars[0];
		tb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		ly_root->addWidget(tb);
	}
	else if(!m_toolBars.isEmpty()) {
		QHBoxLayout *ly1 = new QHBoxLayout(nullptr);
		for(auto tb : m_toolBars) {
			tb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			ly1->addWidget(tb);
		}
		ly1->addStretch();
		ly_root->addLayout(ly1);
	}

	QBoxLayout *ly = new QVBoxLayout();
	ly->setMargin(4);
	ly_root->addLayout(ly);

	if(m_centralWidget) {
		ly->addWidget(m_centralWidget);
		if(!m_centralWidget->windowTitle().isEmpty())
			setWindowTitle(m_centralWidget->windowTitle());
	}
	if(m_dialogButtonBox)
		ly->addWidget(m_dialogButtonBox);

}

qf::qmlwidgets::framework::DialogWidget *Dialog::dialogWidget()
{
	qf::qmlwidgets::framework::DialogWidget *ret = qobject_cast<qf::qmlwidgets::framework::DialogWidget *>(m_centralWidget);
	return ret;
}

void Dialog::showEvent(QShowEvent *ev)
{
	Super::showEvent(ev);
	if(!ev->spontaneous()) {
		// There are two kinds of show events: show events caused by the window system (spontaneous), and internal show events.
		// Spontaneous (QEvent::spontaneous()) show events are sent just after the window system shows the window;
		// they are also sent when a top-level window is redisplayed after being iconified. Internal show events are delivered just before the widget becomes visible.
		if(ev->type() == QEvent::Show) {
			emit visibleChanged(true);
		}
		else if(ev->type() == QEvent::Hide) {
			emit visibleChanged(false);
		}
	}
}

void Dialog::updateCaptionFrame()
{
	qf::qmlwidgets::framework::DialogWidget *dialog_widget = qobject_cast<qf::qmlwidgets::framework::DialogWidget *>(m_centralWidget);
	if(dialog_widget) {
		if(!m_captionFrame)
			m_captionFrame = new qf::qmlwidgets::dialogs::internal::CaptionFrame(this);
		m_captionFrame->setText(dialog_widget->title());
		m_captionFrame->setIconSource(dialog_widget->iconSource());
		m_captionFrame->setIcon(m_captionFrame->createIcon());
		//m_captionFrame->update();
	}
	else {
		QF_SAFE_DELETE(m_captionFrame);
	}
}

void Dialog::setButtonBox(DialogButtonBox *dbb)
{
	qfLogFuncFrame() << dbb;
	if(dbb != m_dialogButtonBox) {
		QF_SAFE_DELETE(m_dialogButtonBox);
		m_dialogButtonBox = dbb;
		if(m_dialogButtonBox) {
			qfDebug() << "\t adding:" << m_dialogButtonBox << "to layout:" << layout();
			/// widget cannot be simply reparented
			/// NULL parent should be set first
			m_dialogButtonBox->setParent(0);
			m_dialogButtonBox->setParent(this);
			updateLayout();
			connect(m_dialogButtonBox, &DialogButtonBox::accepted, this, &Dialog::accept);
			connect(m_dialogButtonBox, &DialogButtonBox::rejected, this, &Dialog::reject);
		}
		emit buttonBoxChanged();
	}
}

void Dialog::setButtons(QDialogButtonBox::StandardButtons buttons)
{
	DialogButtonBox *bbx = buttonBox();
	if(!bbx) {
		bbx = new DialogButtonBox();
		setButtonBox(bbx);
	}
	bbx->setStandardButtons(buttons);
}

