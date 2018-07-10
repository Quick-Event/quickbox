#pragma once

#include "htmlfileexporter.h"

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT StageStartListHtmlExporter : public HtmlFileExporter
{
	Q_OBJECT

	using Super = HtmlFileExporter;
public:
	StageStartListHtmlExporter(QObject *parent = nullptr);
protected:
	void exportClass(int class_id, const QVariantList &class_links) override;
};

}}}

