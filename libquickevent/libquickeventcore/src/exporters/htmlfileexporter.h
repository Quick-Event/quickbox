#pragma once

#include "fileexporter.h"

namespace quickevent {
namespace core {
namespace exporters {

class QUICKEVENTCORE_DECL_EXPORT HtmlFileExporter : public FileExporter
{
	Q_OBJECT

	using Super = FileExporter;
public:
	explicit HtmlFileExporter(QObject *parent = nullptr);
	void generateHtml();
protected:
	void exportClasses();
	virtual void exportClass(int class_id, const QVariantList &class_links) = 0;
};

}}}
