#ifndef QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H
#define QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H

#include "qmlwidgetsglobal.h"

#include <QWidget>

namespace qf {
namespace qmlwidgets {

namespace framework {
class IPersistentOptions;
}

namespace Ui {
class SaveOptionsWidget;
}

class QFQMLWIDGETS_DECL_EXPORT SaveOptionsWidget : public QWidget
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	SaveOptionsWidget(QWidget *parent = nullptr);
	~SaveOptionsWidget() Q_DECL_OVERRIDE;
protected:
	void load_helper(const QVariantMap &m);
	QVariantMap comboToSettings();
	void save_helper(const QVariantMap &all_settings);
	void save();
protected slots:
	void lazyInit();
	void refreshActions();
	void on_btSaveSettings_clicked();
	void on_btDeleteSetting_clicked();
	void onSettingsActivated(int ix);
signals:
	//void settingsActivated(const QVariant &settings);
public:
	void setOptionsProvider(framework::IPersistentOptions *options_provider);
	framework::IPersistentOptions *optionsProvider();

	Q_SLOT void load(bool select_first_option = false);
protected:
	virtual QString persistentOptionsPath();
	/// load/save all options for all the combo keys to application persistent storage
	/// default storage is QSettings
	virtual QString loadPersistentOptionsMap(const QString &path);
	virtual void savePersistentOptionsMap(const QString &path, const QVariant &settings_json);

	/// pull/push options selected in combo from/to optionsProvider()
	virtual QVariant pullPersistentOptions();
	virtual void pushPersistentOptions(const QVariant &opts);
private:
	Ui::SaveOptionsWidget *ui;
	framework::IPersistentOptions *m_optionsProvider;
};

}}

#endif // QF_QMLWIDGETS_SAVEOPTIONS_WIDGET_H

