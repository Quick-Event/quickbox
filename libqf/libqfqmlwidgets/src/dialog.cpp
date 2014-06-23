#include "dialog.h"
#include "frame.h"

#include <qf/core/log.h>

#include <QVBoxLayout>
#include <QSettings>

using namespace qf::qmlwidgets;

Dialog::Dialog(QWidget *parent) :
	QDialog(parent), framework::IPersistentSettings(this)
{
	//connect(this, &Dialog::finished, this, &Dialog::savePersistentSettings);

	m_centralFrame = new Frame(this);
	m_centralFrame->setLayoutType(Frame::LayoutVertical);
	QBoxLayout *ly = new QVBoxLayout(this);
	ly->setMargin(1);
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


