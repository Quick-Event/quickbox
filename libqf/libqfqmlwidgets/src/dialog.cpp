#include "dialog.h"
#include "frame.h"
#include "dialogbuttonbox.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QVBoxLayout>
#include <QSettings>

using namespace qf::qmlwidgets;

Dialog::Dialog(QWidget *parent) :
	QDialog(parent), framework::IPersistentSettings(this)
{
	qfLogFuncFrame();
	//connect(this, &Dialog::finished, this, &Dialog::savePersistentSettings);
	m_dialogButtonBox = nullptr;
	m_centralFrame = new Frame(this);
	m_centralFrame->setLayoutType(Frame::LayoutVertical);
	QBoxLayout *ly = new QVBoxLayout(this);
	//ly->setMargin(1);
	qfDebug() << "\t adding:" << m_centralFrame << "to layout:" << ly;
	ly->addWidget(m_centralFrame);
	setLayout(ly);
}

Dialog::~Dialog()
{
	savePersistentSettings();
}
/*
int Dialog::exec()
{
	loadPersistentSettings();
	int ret = QDialog::exec();
	savePersistentSettings();
	return ret;
}
*/
QQmlListProperty<QWidget> Dialog::widgets()
{
	return m_centralFrame->widgets();
}

void Dialog::loadPersistentSettings(bool recursively)
{
	qfLogFuncFrame() << recursively;
	if(recursively) {
		loadPersistentSettingsRecursively();
	}
	else {
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
			layout()->addWidget(m_dialogButtonBox);
			connect(m_dialogButtonBox, &DialogButtonBox::accepted, this, &Dialog::accept);
			connect(m_dialogButtonBox, &DialogButtonBox::rejected, this, &Dialog::reject);
		}
		emit buttonBoxChanged();
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

void Dialog::classBegin()
{
	qfLogFuncFrame();
}

void Dialog::componentComplete()
{
	qfLogFuncFrame();
	loadPersistentSettings(true);
}



