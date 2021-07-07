#pragma once

#include <plugins/Event/src/services/service.h>

class QTimer;

namespace Runs {
namespace services {

class ResultsExporterSettings : public Event::services::ServiceSettings
{
	using Super = Event::services::ServiceSettings;

public:
	enum class OutputFormat {HtmlMulti = 0, CSOS, CSV, IofXml3, COUNT};

	QF_VARIANTMAP_FIELD(QString, e, setE, xportDir)
	QF_VARIANTMAP_FIELD2(int, e, setE, xportIntervalSec, 0)
	QF_VARIANTMAP_FIELD(QString, w, setW, henFinishedRunCmd)
	QF_VARIANTMAP_FIELD2(int, o, setO, utputFormat, static_cast<int>(OutputFormat::HtmlMulti))

public:
	ResultsExporterSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class ResultsExporter : public Event::services::Service
{
	Q_OBJECT

	using Super = Event::services::Service;
public:
	ResultsExporter(QObject *parent);

	void loadSettings() override;
	ResultsExporterSettings settings() const {return ResultsExporterSettings(m_settings);}

	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;

	static QString serviceName();

	bool exportResults();
private:
	void onExportTimerTimeOut();
	void init();
private:
	QTimer *m_exportTimer = nullptr;
};

}}

