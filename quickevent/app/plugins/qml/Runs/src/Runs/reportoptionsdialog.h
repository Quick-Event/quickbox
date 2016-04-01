#ifndef RUNS_REPORTOPTIONSDIALOG_H
#define RUNS_REPORTOPTIONSDIALOG_H

#include <qf/qmlwidgets/framework/ipersistentsettings.h>

#include <qf/core/utils.h>

#include <QDialog>
#include <QVariantMap>

namespace Runs {

namespace Ui {
class ReportOptionsDialog;
}

class ReportOptionsDialog : public QDialog, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT

	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
private:
	using Super = QDialog;
public:
	enum class BreakType : int {None = 0, Column, Page};
	enum class FilterType : int {WildCard = 0, RegExp, ClassName};
public:
	explicit ReportOptionsDialog(QWidget *parent = 0);
	~ReportOptionsDialog();

	int exec() Q_DECL_OVERRIDE;

	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();

	void setClassNamesFilter(const QStringList &class_names);

	BreakType breakType() const;
	//Q_INVOKABLE QVariantMap optionsToMap() const;
	Q_INVOKABLE bool isBreakAfterEachClass() const {return breakType() != BreakType::None;}
	Q_INVOKABLE bool isColumnBreak() const {return breakType() == BreakType::Column;}
	Q_INVOKABLE QString sqlWhereExpression() const;
private:
	class Options : public QVariantMap
	{
		QF_VARIANTMAP_FIELD2(int, b, setB, reakType, 0)
		QF_VARIANTMAP_FIELD(QString, c, setC, lassFilter)
		QF_VARIANTMAP_FIELD2(int, c, setC, lassFilterType, 0)
		QF_VARIANTMAP_FIELD(bool, is, set, UseClassFilter)
		QF_VARIANTMAP_FIELD(bool, is, set, InvertClassFilter)
		public:
			Options(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}
	};
private:
	Ui::ReportOptionsDialog *ui;
};


} // namespace Runs
#endif // RUNS_REPORTOPTIONSDIALOG_H
