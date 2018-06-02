#ifndef EVENTSTATISTICSOPTIONS_H
#define EVENTSTATISTICSOPTIONS_H

#include <qf/qmlwidgets/framework/ipersistentsettings.h>

#include <qf/core/utils.h>

#include <QDialog>

namespace Ui {
class EventStatisticsOptions;
}

class EventStatisticsOptions : public QDialog, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
private:
	using Super = QDialog;
public:
	explicit EventStatisticsOptions(QWidget *parent = 0);
	~EventStatisticsOptions();

	QString persistentSettingsPath() Q_DECL_OVERRIDE {return staticPersistentSettingsPath();}
	static QString staticPersistentSettingsPath();

	int exec() Q_DECL_OVERRIDE;
	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();

	class Options : public QVariantMap
	{
		QF_VARIANTMAP_FIELD2(bool, is, set, ShowPrintDialog, true)
		QF_VARIANTMAP_FIELD2(int, a, setA, utoRefreshSec, 10)
		QF_VARIANTMAP_FIELD2(int, a, setA, utoPrintNewRunners, 5)
		QF_VARIANTMAP_FIELD2(int, a, setA, utoPrintNewMin, 15)
		public:
			Options(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}
	};
private:
	Ui::EventStatisticsOptions *ui;
};

#endif // EVENTSTATISTICSOPTIONS_H
