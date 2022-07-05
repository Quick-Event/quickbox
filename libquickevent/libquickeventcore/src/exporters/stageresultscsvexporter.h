#pragma once

#include "fileexporter.h"
#include <QTextStream>

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT StageResultsCsvExporter : public FileExporter
{
	Q_OBJECT

	using Super = FileExporter;
public:
	StageResultsCsvExporter(QObject *parent = nullptr);
	void generateCsvMulti();
	void generateCsvSingle();
	void setSeparator(QChar sep) { separator = sep;}
protected:
	void exportClasses(bool single_file);
	void exportClass(int class_id, QTextStream &csv);
	void exportCsvHeader(QTextStream &csv);
	QChar separator = ';';
};

}}}

