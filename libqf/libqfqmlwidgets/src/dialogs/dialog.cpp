#include "dialog.h"
#include "../frame.h"
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
	//connect(this, &Dialog::finished, this, &Dialog::savePersistentSettings);
}

Dialog::~Dialog()
{
	savePersistentSettings();
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




