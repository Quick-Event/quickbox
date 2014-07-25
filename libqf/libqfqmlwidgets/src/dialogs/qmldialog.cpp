#include "qmldialog.h"
#include "../frame.h"
#include "../dialogbuttonbox.h"

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
	m_dialogButtonBox = nullptr;
	m_centralFrame = new Frame(this);
	m_centralFrame->setLayoutType(Frame::LayoutVertical);
	QBoxLayout *ly = new QVBoxLayout(this);
	//ly->setMargin(1);
	qfDebug() << "\t adding:" << m_centralFrame << "to layout:" << ly;
	ly->addWidget(m_centralFrame);
	setLayout(ly);
}

QmlDialog::~QmlDialog()
{
}

QQmlListProperty<QWidget> QmlDialog::widgets()
{
	return m_centralFrame->widgets();
}

void QmlDialog::setButtonBox(DialogButtonBox *dbb)
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

void QmlDialog::classBegin()
{
	qfLogFuncFrame();
}

void QmlDialog::componentComplete()
{
	qfLogFuncFrame();
	loadPersistentSettingsRecursively();
}



