#include "qmldialog.h"
#include "../frame.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QVBoxLayout>
#include <QSettings>

using namespace qf::qmlwidgets::dialogs;

QmlDialog::QmlDialog(QWidget *parent) :
	Super(parent)
{
	qfLogFuncFrame();
	//connect(this, &QmlDialog::finished, this, &QmlDialog::savePersistentSettings);
	m_centralFrame = new Frame(nullptr);
	setCentralWidget(m_centralFrame);
}

QmlDialog::~QmlDialog()
{
}

QQmlListProperty<QWidget> QmlDialog::widgets()
{
	return m_centralFrame->widgets();
}

void QmlDialog::classBegin()
{
	qfLogFuncFrame();
}

void QmlDialog::componentComplete()
{
	qfLogFuncFrame();
	loadPersistentSettingsRecursively();
}



