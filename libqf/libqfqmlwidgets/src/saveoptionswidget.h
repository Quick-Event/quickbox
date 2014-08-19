#ifndef QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H
#define QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H

#include "qmlwidgetsglobal.h"

#include <QWidget>

namespace qf {
namespace qmlwidgets {

namespace Ui {
class SaveOptionsWidget;
}

class QFQMLWIDGETS_DECL_EXPORT SaveOptionsWidget : public QWidget
{
	Q_OBJECT
	// urcuje, jestli se ve funkci load jako defaultni nastaveni pouzije prazdne nebo prvni z ulozenych, default je false
	//Q_PROPERTY(bool loadFirstSavedSettingsOnCreate READ isLoadFirstSavedSettingsOnCreate WRITE setLoadFirstSavedSettingsOnCreate)
	//QF_FIELD_RW(bool, is, set, LoadFirstSavedSettingsOnCreate);
private:
	typedef QWidget Super;
public:
	SaveOptionsWidget(QWidget *parent = NULL);
	~SaveOptionsWidget() Q_DECL_OVERRIDE;
protected:
	//QFSettingsProviderInterface *f_settingsProvider;
protected:
	void load_helper(const QVariantMap &m);
	QVariantMap comboToSettings();
	void save_helper(const QVariantMap &all_settings);
	void save();
	//virtual QFXmlConfig* userConfig(bool throw_exc = Qf::ThrowExc);
	//virtual QFXmlConfig* appConfig(bool throw_exc = Qf::ThrowExc);
	//bool isAppHasUserConfig();
	//bool currentUserHasGrantForGlobalSettins();
protected slots:
	void lazyInit();
	void refreshActions();
	void on_btSaveSettings_clicked();
	//void on_btSaveSettingsGlobal_clicked();
	void on_btDeleteSetting_clicked();
	void onSettingsActivated(int ix);
signals:
	//void settingsActivated(const QVariant &settings);
public:
	void setOptionsProvider(QObject *options_provider);
	QObject *optionsProvider();

	Q_SLOT void load();
private:
	virtual QString loadPersistentSettings(const QString &path);
	virtual void savePersistentSettings(const QString &path, const QVariant &settings_json);
	QVariant pullPersistentOptions();
	void pushPersistentOptions(const QVariant &opts);
private:
	Ui::SaveOptionsWidget *ui;
	QObject *m_optionsProvider;
};

}}

#endif // QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H

