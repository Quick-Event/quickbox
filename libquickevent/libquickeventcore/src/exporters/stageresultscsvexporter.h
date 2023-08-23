#pragma once

#include "fileexporter.h"

class QChar;
class QTextStream;

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT StageResultsCsvExporter : public FileExporter
{
	Q_OBJECT

	using Super = FileExporter;
public:
	QF_PROPERTY_IMPL(QString, o, O, utFile)
	QF_PROPERTY_IMPL(bool, s, S, implePath)
	QF_PROPERTY_IMPL(bool, w, W, ithDidNotStart)
public:
	StageResultsCsvExporter(bool is_iof_race = false, QObject *parent = nullptr);
	void generateCsvMulti();
	void generateCsvSingle();
	void setSeparator(QChar sep) { m_separator = sep; }
protected:
	void exportClasses(bool single_file);
	void exportClass(int class_id, QTextStream &csv);
	void exportCsvHeader(QTextStream &csv);
private:
	QChar m_separator = ';';
	bool m_isIofRace = false;
};

}}}

