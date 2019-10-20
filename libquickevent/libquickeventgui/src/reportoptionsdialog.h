#pragma once

#include "quickeventguiglobal.h"

#include <qf/qmlwidgets/framework/ipersistentsettings.h>

#include <qf/core/utils.h>

#include <QDialog>
#include <QVariantMap>

namespace quickevent {
namespace gui {

namespace Ui {
class ReportOptionsDialog;
}

class QUICKEVENTGUI_DECL_EXPORT ReportOptionsDialog : public QDialog, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT

	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId NOTIFY persistentSettingsIdChanged)
	Q_PROPERTY(bool classFilterVisible READ isClassFilterVisible WRITE setClassFilterVisible NOTIFY classFilterVisibleChanged)
	Q_PROPERTY(bool startListOptionsVisible READ isStartListOptionsVisible WRITE setStartListOptionsVisible NOTIFY startListOptionsVisibleChanged)
	Q_PROPERTY(bool startersOptionsVisible READ isStartersOptionsVisible WRITE setStartersOptionsVisible NOTIFY startersOptionsVisibleChanged)
	Q_PROPERTY(bool vacantsVisible READ isVacantsVisible WRITE setVacantsVisible NOTIFY vacantsVisibleChanged)
	Q_PROPERTY(bool stagesOptionVisible READ isStagesOptionVisible WRITE setStagesOptionVisible NOTIFY stagesOptionVisibleChanged)
	Q_PROPERTY(int stagesCount READ stagesCount WRITE setStagesCount NOTIFY stagesCountChanged)
	Q_PROPERTY(bool pageLayoutVisible READ isPageLayoutVisible WRITE setPageLayoutVisible NOTIFY pageLayoutVisibleChanged)
	Q_PROPERTY(bool columnCountEnable READ isColumnCountEnable WRITE setColumnCountEnable NOTIFY columnCountEnableChanged)
	Q_PROPERTY(bool resultOptionsVisible READ isResultOptionsVisible WRITE setResultOptionsVisible NOTIFY resultOptionsVisibleChanged)

	QF_PROPERTY_BOOL_IMPL2(c, C, lassFilterVisible, true)
	QF_PROPERTY_BOOL_IMPL2(s, S, tartListOptionsVisible, false)
	QF_PROPERTY_BOOL_IMPL2(s, S, tartersOptionsVisible, false)
	QF_PROPERTY_BOOL_IMPL2(v, V, acantsVisible, true)
	QF_PROPERTY_BOOL_IMPL2(s, S, tagesOptionVisible, false)
	QF_PROPERTY_IMPL2(int,s, S, tagesCount, 1)
	QF_PROPERTY_BOOL_IMPL2(p, P, ageLayoutVisible, false)
	QF_PROPERTY_BOOL_IMPL2(c, C, olumnCountEnable, true)
	QF_PROPERTY_BOOL_IMPL2(r, R, esultOptionsVisible, false)
private:
	using Super = QDialog;
public:
	enum class BreakType : int {None = 0, Column, Page};
	enum class FilterType : int {WildCard = 0, RegExp, ClassName};

	class Options : public QVariantMap
	{
		QF_VARIANTMAP_FIELD2(int, s, setS, tartersOptionsLineSpacing, 0)
		QF_VARIANTMAP_FIELD2(int, b, setB, reakType, 0)
		QF_VARIANTMAP_FIELD2(QString, c, setC, olumns, "%,%")
		QF_VARIANTMAP_FIELD2(int, p, setP, ageWidth, 210)
		QF_VARIANTMAP_FIELD2(int, p, setP, ageHeight, 297)
		QF_VARIANTMAP_FIELD2(int, c, setC, olumnCount, 2)
		QF_VARIANTMAP_FIELD2(int, h, setH, orizontalMargin, 10)
		QF_VARIANTMAP_FIELD2(int, v, setV, erticalMargin, 5)
		QF_VARIANTMAP_FIELD2(bool, is, set, ShirinkPageWidthToColumnCount, false)
		QF_VARIANTMAP_FIELD(QString, c, setC, lassFilter)
		QF_VARIANTMAP_FIELD2(int, c, setC, lassFilterType, 0)
		QF_VARIANTMAP_FIELD(bool, is, set, UseClassFilter)
		QF_VARIANTMAP_FIELD(bool, is, set, InvertClassFilter)
		QF_VARIANTMAP_FIELD(bool, is, set, StartListPrintVacants)
		QF_VARIANTMAP_FIELD(bool, is, set, StartListPrintStartNumbers)
		QF_VARIANTMAP_FIELD2(int, s, setS, tagesCount, 1)
		QF_VARIANTMAP_FIELD2(int, r, setR, esultNumPlaces, 9999)
		public:
			Options(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}
	};
public:
	explicit ReportOptionsDialog(QWidget *parent = nullptr);
	~ReportOptionsDialog() override;

	int exec() Q_DECL_OVERRIDE;

	QString persistentSettingsPath() Q_DECL_OVERRIDE;
	bool setPersistentSettingsId(const QString &id) Q_DECL_OVERRIDE;
	Q_SIGNAL void persistentSettingsIdChanged(const QString &id);

	void setOptions(const Options &options);
	Options options() const;
	Q_SLOT QVariantMap optionsMap() const {return options();}
	static Options savedOptions(const QString &persistent_settings_id = QString());

	Q_INVOKABLE QVariantMap reportProperties() const;

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();

	void setClassNamesFilter(const QStringList &class_names);

	BreakType breakType() const;
	//Q_INVOKABLE QVariantMap optionsToMap() const;
	Q_INVOKABLE bool isStartListPrintVacants() const;
	Q_INVOKABLE bool isStartListPrintStartNumbers() const;
	Q_INVOKABLE bool isBreakAfterEachClass() const {return breakType() != BreakType::None;}
	Q_INVOKABLE bool isColumnBreak() const {return breakType() == BreakType::Column;}
	Q_INVOKABLE int resultNumPlaces() const;
	Q_INVOKABLE QString sqlWhereExpression() const;
	static QString sqlWhereExpression(const Options &opts);
protected:
	//void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
private:
	Ui::ReportOptionsDialog *ui;
};


}}
