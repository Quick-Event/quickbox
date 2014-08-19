#include "saveoptionswidget.h"
#include "ui_saveoptionswidget.h"

#include "framework/ipersistentsettings.h"

#include <qf/core/assert.h>
#include <qf/core/log.h>
#include <qf/core/utils/fileutils.h>

#include <QTimer>
#include <QStyle>
#include <QSettings>
#include <QJsonDocument>

using namespace qf::qmlwidgets;
//=================================================
//             SaveOptionsWidget
//=================================================
namespace {
const char *OPTIONS_GETTER_SIGNATURE = "persistentOptions()";
const char *OPTIONS_SETTER_SIGNATURE = "setPersistentOptions(QVariant)";
const char *OPTIONS_PERSISTENT_SETTING_KEY = "persistentOptions";
}
SaveOptionsWidget::SaveOptionsWidget(QWidget *parent)
	: Super(parent), m_optionsProvider(nullptr)
{
	ui = new Ui::SaveOptionsWidget;
	ui->setupUi(this);

	QStyle *sty = style();
	ui->btDeleteSetting->setIcon(sty->standardIcon(QStyle::SP_DialogDiscardButton));
	ui->btSaveSettings->setIcon(sty->standardIcon(QStyle::SP_DialogSaveButton));

	ui->lstSettings->clear();
	connect(ui->lstSettings, SIGNAL(activated(int)), this, SLOT(onSettingsActivated(int)));
	QTimer::singleShot(0, this, SLOT(lazyInit()));
}

SaveOptionsWidget::~SaveOptionsWidget()
{
	delete ui;
}

void SaveOptionsWidget::lazyInit()
{
	refreshActions();
}

void SaveOptionsWidget::refreshActions()
{
	//bool is_superuser = currentUserHasGrantForGlobalSettins();
	//ui->btSaveSettingsGlobal->setVisible(isAppHasUserConfig() && is_superuser);
}

static const int SETTINGS_VALUE_ROLE = Qt::UserRole;

void SaveOptionsWidget::load_helper(const QVariantMap& m)
{
	qfLogFuncFrame();// << "is global:" << is_global;
	//qfInfo() << QFJson::variantToString(m);
	QComboBox *lst = ui->lstSettings;
	lst->blockSignals(true);
	foreach(QString s, m.keys()) {
		if(lst->findText(s) >= 0) continue;
		qfDebug() << "\t" << s;
		int ix;
		if(s == "default") {lst->insertItem(0, s); ix = 0;}
		else {lst->addItem(s); ix = lst->count() - 1;}
		//qfInfo() << "adding filter name:" << s << "to index:" << ix;
		lst->setItemData(ix, m.value(s), SETTINGS_VALUE_ROLE);
	}
	lst->blockSignals(false);

}

void SaveOptionsWidget::load()
{
	qfLogFuncFrame();
	QComboBox *cbx = ui->lstSettings;
	QString orig_text = cbx->currentText();
	//qfWarning() << "orig_text:" << orig_text << "index:" << cbx->currentIndex();
	cbx->clear();
	QF_ASSERT(optionsProvider(),
			  "options provider not set",
			  return);
	framework::IPersistentSettings *persistent_settings_provider = dynamic_cast<framework::IPersistentSettings*>(optionsProvider());
	QF_ASSERT(persistent_settings_provider,
			  QString("options provider %1 must implement framework::IPersistentSettings interface").arg(optionsProvider()->metaObject()->className()),
			  return);
	QString persistent_settings_path = persistent_settings_provider->persistentSettingsPath();
	QF_ASSERT(!persistent_settings_path.isEmpty(),
			  QString("Persistent settings path of options provider %1 is empty").arg(optionsProvider()->metaObject()->className()),
			  return);
	persistent_settings_path = qf::core::utils::FileUtils::joinPath(persistent_settings_path, OPTIONS_PERSISTENT_SETTING_KEY);
	QByteArray json = loadPersistentSettings(persistent_settings_path).toUtf8();
	QJsonDocument json_doc;
	json_doc.fromJson(json);
	QVariantMap m = json_doc.toVariant().toMap();
	load_helper(m);
	if(!orig_text.isEmpty()) {
		int ix = cbx->findText(orig_text);
		//qfInfo() << "index of:" << orig_text << "is:" << ix;
		if(ix < 0)
			ix = 0;
		//if(ix == cbx->currentIndex()) onSettingsActivated(ix);
		cbx->setCurrentIndex(ix);
		onSettingsActivated(ix);
	}
	else {
		cbx->setCurrentIndex(-1);
	}
}

QString SaveOptionsWidget::loadPersistentSettings(const QString &path)
{
	QSettings settings;
	QString ret = settings.value(path).toString();
	return ret;
}

void SaveOptionsWidget::savePersistentSettings(const QString &path, const QVariant &settings_json)
{
	QSettings settings;
	settings.setValue(path, settings_json);
}

QVariant SaveOptionsWidget::pullPersistentOptions()
{
	QVariant ret;
	QObject *op = optionsProvider();
	if(op) {
		bool ok = QMetaObject::invokeMethod(op, OPTIONS_GETTER_SIGNATURE, Qt::DirectConnection,
		                          Q_RETURN_ARG(QVariant, ret));
		if(!ok)
			qfWarning() << "Error invoking" << OPTIONS_GETTER_SIGNATURE;
	}
	return ret;
}

void SaveOptionsWidget::pushPersistentOptions(const QVariant &opts)
{
	QObject *op = optionsProvider();
	if(op) {
		bool ok = QMetaObject::invokeMethod(op, OPTIONS_SETTER_SIGNATURE, Qt::DirectConnection,
											Q_ARG(QVariant, opts));
		if(!ok)
			qfWarning() << "Error invoking" << OPTIONS_SETTER_SIGNATURE;
	}
}

void SaveOptionsWidget::save()
{
	QComboBox *cbx = ui->lstSettings;
	QString option_name = cbx->currentText().trimmed();
	if(option_name.isEmpty()) {
		option_name = "default";
	}
	QVariantMap named_options = comboToSettings();
	QVariant options = pullPersistentOptions();
	named_options[option_name] = options;
	save_helper(named_options);
	load();
}

QVariantMap SaveOptionsWidget::comboToSettings()
{
	QVariantMap all_settings;
	{
		QComboBox *cbx = ui->lstSettings;
		for(int i=0; i<cbx->count(); i++) {
			QVariant v = cbx->itemData(i, SETTINGS_VALUE_ROLE);
			all_settings[cbx->itemText(i)] = v;
		}
	}
	return all_settings;
}

void SaveOptionsWidget::save_helper(const QVariantMap& all_settings)
{
	QF_ASSERT(optionsProvider(),
			  "options provider not set",
			  return);
	framework::IPersistentSettings *persistent_settings_provider = dynamic_cast<framework::IPersistentSettings*>(optionsProvider());
	QF_ASSERT(persistent_settings_provider,
			  QString("options provider %1 must implement framework::IPersistentSettings interface").arg(optionsProvider()->metaObject()->className()),
			  return);
	QString persistent_settings_path = persistent_settings_provider->persistentSettingsPath();
	QF_ASSERT(!persistent_settings_path.isEmpty(),
			  QString("Persistent settings path of options provider %1 is empty").arg(optionsProvider()->metaObject()->className()),
			  return);
	persistent_settings_path = qf::core::utils::FileUtils::joinPath(persistent_settings_path, OPTIONS_PERSISTENT_SETTING_KEY);

	QJsonDocument json_doc;
	json_doc.fromVariant(all_settings);
	QString json_str = QString::fromUtf8(json_doc.toJson());
	savePersistentSettings(persistent_settings_path, json_str);
}

void SaveOptionsWidget::on_btSaveSettings_clicked()
{
	save();
}

void SaveOptionsWidget::on_btDeleteSetting_clicked()
{
	QComboBox *cbx = ui->lstSettings;
	int ix = cbx->currentIndex();
	if(ix < 0) return;
	cbx->removeItem(ix);
	QVariantMap all_settings = comboToSettings();
	save_helper(all_settings);
	load();
}
/*
void SaveOptionsWidget::setSettingsProvider(QFSettingsProviderInterface* settings_provider)
{
	f_settingsProvider = settings_provider;
	//qfInfo() << f_settingsProvider;
	//QTimer::singleShot(0, this, SLOT(load()));
}

QFSettingsProviderInterface* SaveOptionsWidget::optionsProvider()
{
	if(!f_settingsProvider) {
		qfWarning() << "Settings provider is NULL.";
		qfInfo() << QFLog::stackTrace();
	}
	return f_settingsProvider;
}
*/
void SaveOptionsWidget::onSettingsActivated(int ix)
{
	qfLogFuncFrame() << "ix:" << ix;
	QComboBox *cbx = ui->lstSettings;
	if(ix >= 0 && ix < cbx->count()) {
		QVariant opts = cbx->itemData(ix, SETTINGS_VALUE_ROLE);
		pushPersistentOptions(opts);
	}
}

void SaveOptionsWidget::setOptionsProvider(QObject *options_provider)
{
	m_optionsProvider = options_provider;
	if(m_optionsProvider) {
		int ix;
		ix = m_optionsProvider->metaObject()->indexOfMethod(OPTIONS_GETTER_SIGNATURE);
		QF_CHECK(ix >= 0,
				 QString("Options providers shall have a getter: %1").arg(OPTIONS_GETTER_SIGNATURE));
		ix = m_optionsProvider->metaObject()->indexOfMethod(OPTIONS_SETTER_SIGNATURE);
		QF_CHECK(ix >= 0,
				 QString("Options providers shall have a setter: %1").arg(OPTIONS_SETTER_SIGNATURE));
	}
}

QObject *SaveOptionsWidget::optionsProvider()
{
	return m_optionsProvider;
}
