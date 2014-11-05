#include "dialog.h"
#include "../frame.h"
#include "../framework/dialogwidget.h"
#include "../menubar.h"
#include "../toolbar.h"
#include "../dialogbuttonbox.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QVBoxLayout>
#include <QSettings>

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
		if(dialog_widget)
			dialog_widget->updateDialogUi(this);
		updateLayout();
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

void Dialog::loadPersistentSettings()
{
	QString path = persistentSettingsPath();
	qfDebug() << "\t" << path;
	if(!path.isEmpty()) {
		QSettings settings;
		settings.beginGroup(path);
		QRect geometry = settings.value("geometry").toRect();
		if(geometry.isValid()) {
			this->setGeometry(geometry);
		}
	}
}

void Dialog::setDoneCancelled(bool b)
{
	if(b != m_doneCancelled) {
		m_doneCancelled = b;
		emit doneCancelledChanged(m_doneCancelled);
	}
}

void Dialog::done(int result)
{
	qfLogFuncFrame() << result;
	setDoneCancelled(false);
	emit aboutToBeDone(result);
	if(!isDoneCancelled()) {
		Super::done(result);
	}
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
	QBoxLayout *ly = qobject_cast<QBoxLayout*>(this->layout());
	if(!ly) {
		QLayout *ly2 = layout();
		QF_SAFE_DELETE(ly2);
		ly = new QVBoxLayout(this);
		setLayout(ly);
	}
	else {
		while(ly->itemAt(0)) {
			ly->removeItem(ly->itemAt(0));
		}
	}

	if(m_menuBar)
		ly->addWidget(m_menuBar);

	if(m_toolBars.count() == 1) {
		ToolBar *tb = m_toolBars[0];
		tb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		ly->addWidget(tb);
	}
	else if(!m_toolBars.isEmpty()) {
		QHBoxLayout *ly1 = new QHBoxLayout(nullptr);
		for(auto tb : m_toolBars) {
			tb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			ly1->addWidget(tb);
		}
		ly1->addStretch();
		ly->addLayout(ly1);
	}

	if(m_centralWidget) {
		ly->addWidget(m_centralWidget);
		if(!m_centralWidget->windowTitle().isEmpty())
			setWindowTitle(m_centralWidget->windowTitle());
	}
	if(m_dialogButtonBox)
		ly->addWidget(m_dialogButtonBox);
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




