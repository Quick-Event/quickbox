#include "receiptsprinter.h"
#include "Receipts/receiptsplugin.h"

#include <qf/core/collator.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/reports/processor/reportpainter.h>
#include <qf/qmlwidgets/reports/processor/reportprocessor.h>

#include <QDomDocument>
#include <QPrinter>
#include <QPrinterInfo>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;

ReceiptsPrinter::ReceiptsPrinter(const ReceiptsPrinterOptions &opts, QObject *parent)
	: QObject(parent)
	, m_printerOptions(opts)
{
}
/*
static Receipts::ReceiptsPlugin *receiptsPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto *ret = qobject_cast<Receipts::ReceiptsPlugin *>(fwk->plugin("Receipts"));
	QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}
*/
void ReceiptsPrinter::printReceipt(const QString &report_file_name, const QVariantMap &report_data)
{
	qfLogFuncFrame();
	QF_TIME_SCOPE("ReceiptsPrinter::printReceipt()");
	const ReceiptsPrinterOptions &printer_opts = m_printerOptions;
	QPrinter *printer = nullptr;
	QPaintDevice *paint_device = nullptr;
	if(printer_opts.printerType() == (int)ReceiptsPrinterOptions::PrinterType::GraphicPrinter) {
		QF_TIME_SCOPE("init graphics printer");
		QPrinterInfo pi = QPrinterInfo::printerInfo(printer_opts.graphicsPrinterName());
		if(pi.isNull()) {
			for(auto s : QPrinterInfo::availablePrinterNames()) {
				qfInfo() << "available printer:" << s;
			}
			pi = QPrinterInfo::defaultPrinter();
		}
		if(pi.isNull()) {
			qfWarning() << "Default printer not set";
			return;
		}
		qfInfo() << "printing on:" << pi.printerName();
		printer = new QPrinter(pi);
		paint_device = printer;
	}
	else {
		qfInfo() << "printing on:" << printer_opts.characterPrinterModel() << "at:" << printer_opts.characterPrinterDevice();
		qff::MainWindow *fwk = qff::MainWindow::frameWork();
		paint_device = fwk;
	}
	qf::qmlwidgets::reports::ReportProcessor rp(paint_device);
	{
		QF_TIME_SCOPE("setting report and data");
		rp.setReport(report_file_name);
		for(auto key : report_data.keys()) {
			rp.setTableData(key, report_data.value(key));
		}
	}
	if(printer_opts.printerType() == (int)ReceiptsPrinterOptions::PrinterType::GraphicPrinter) {
		QF_TIME_SCOPE("process graphics");
		{
			QF_TIME_SCOPE("process report");
			rp.process();
		}
		qf::qmlwidgets::reports::ReportItemMetaPaintReport *doc;
		{
			QF_TIME_SCOPE("getting processor output");
			doc = rp.processorOutput();
		}
		qf::qmlwidgets::reports::ReportItemMetaPaint *it = doc->child(0);
		if(it) {
			QF_TIME_SCOPE("draw meta-paint");
			qf::qmlwidgets::reports::ReportPainter painter(paint_device);
			painter.drawMetaPaint(it);
		}
		QF_SAFE_DELETE(printer);
	}
	else {
		QDomDocument doc;
		doc.setContent(QLatin1String("<?xml version=\"1.0\"?><report><body/></report>"));
		QDomElement el_body = doc.documentElement().firstChildElement("body");
		qf::qmlwidgets::reports::ReportProcessor::HtmlExportOptions opts;
		opts.setConvertBandsToTables(false);
		rp.processHtml(el_body, opts);
		//qfInfo() << doc.toString();
		QList<QByteArray> ba_lst = createPrinterData(el_body, printer_opts);
		QFile f(printer_opts.characterPrinterDevice());
		//QFile f("/home/fanda/t/receipt.txt");
		if(f.open(QFile::WriteOnly)) {
			for(QByteArray ba : ba_lst) {
				f.write(ba);
				f.write("\n");
			}
		}
		else {
			qfError() << "Cannot open file" << f.fileName() << "for writing!";
		}
	}
}

static const QByteArray epson_commands[] =
{
	QByteArray("\033E\x01"),		//    BoldOn
	QByteArray("\033E\x00", 3),		//    BoldOff
	QByteArray("\033-\x01"),		//    UnderlineOn
	QByteArray("\033-\x00", 3),		//    UnderlineOff
	QByteArray("\033-\x02"),		//    Underline2On
	QByteArray("\033-\x00", 3),		//    Underline2Off
	QByteArray("\033a\x00", 3),		//    AlignLeft
	QByteArray("\033a\x01", 3),		//    AlignCenter
	QByteArray("\033a\x02", 3),		//    AlignRight
	QByteArray("\033@"),			//    Init
	QByteArray("\035V\102\x00", 4),	//    Cut
	QByteArray("\n"),				//    Eoln
};

struct PrintData
{
	enum class Command : int {
		BoldOn = 0,
		BoldOff,
		UnderlineOn,
		UnderlineOff,
		Underline2On,
		Underline2Off,
		AlignLeft,
		AlignCenter,
		AlignRight,
		Init,
		Cut,
		Eoln,
		HorizontalLine,
		Text,
	};
	Command command;
	QByteArray data;
	int width = 0;
	Qt::Alignment alignment = Qt::AlignLeft;

	PrintData(Command cmd, const QByteArray &data = QByteArray(), int width = 0, Qt::Alignment alignment = Qt::AlignLeft)
		: command(cmd), data(data), width(width), alignment(alignment)
	{}
	bool isCommand() const {return command != Command::Text;}
	int textLength() const {return isCommand()? 0: data.length();}
	const QByteArray toByteArray() const
	{
		QByteArray ret;
		switch(command) {
		case Command::BoldOn: ret += "<B>"; break;
		case Command::BoldOff: ret += "</B>"; break;
		case Command::UnderlineOn: ret += "<U>"; break;
		case Command::UnderlineOff: ret += "</U>:"; break;
		case Command::Underline2On: ret += "<u>:"; break;
		case Command::Underline2Off: ret += "</u>:"; break;
		case Command::AlignLeft: ret += "<|-->"; break;
		case Command::AlignCenter: ret += "<-|->"; break;
		case Command::AlignRight: ret += "<--|>"; break;
		case Command::Init: ret += "<INIT>"; break;
		case Command::Cut: ret += "<CUT>"; break;
		case Command::Eoln: ret += "<EOL>"; break;
		case Command::HorizontalLine: ret += "<HR>"; break;
		case Command::Text:
			if(width == 0)
				ret += data;
			else
				ret += "<" + (width < 0? "%:": QByteArray::number(width) + ":") + data + ">";
			break;
		}
		return ret;
	}
};
typedef QList<PrintData> PrintLine;

class DirectPrintContext
{
public:
	PrintLine line;
	int horizontalLayoutNestCount = 0;
	int printerLineWidth = 42;
};

void ReceiptsPrinter::createPrinterData_helper(const QDomElement &el, DirectPrintContext *print_context)
{
	//QByteArray text;
	PrintLine pre_commands;
	PrintLine post_commands;
	int text_width = 0;
	bool is_halign = el.tagName() == QLatin1String("div")
			&& el.attribute(qf::qmlwidgets::reports::ReportProcessor::HTML_ATTRIBUTE_LAYOUT) == QLatin1String("horizontal");
	if(is_halign)
		print_context->horizontalLayoutNestCount++;
	QDomNamedNodeMap attrs = el.attributes();
	for (int i = 0; i < attrs.count(); ++i) {
		QDomAttr attr = attrs.item(i).toAttr();
		QString key = attr.name();
		if(key == QLatin1String("lpt_textWidth")) {
			QString val = attr.value();
			if(val == "%")
				text_width = -1;
			else
				text_width = val.toInt();
		}
		else if(key == QLatin1String("lpt_textStyle")) {
			QStringList styles = attr.value().split(',', QString::SkipEmptyParts);
			for(QString s : styles) {
				s = s.trimmed();
				if(s == QLatin1String("bold")) {
					pre_commands << PrintData(PrintData::Command::BoldOn);
					post_commands.insert(0, PrintData(PrintData::Command::BoldOff));
				}
				else if(s == QLatin1String("underline")) {
					pre_commands << PrintData(PrintData::Command::UnderlineOn);
					post_commands.insert(0, PrintData(PrintData::Command::UnderlineOff));
				}
				else if(s == QLatin1String("underline2")) {
					pre_commands << PrintData(PrintData::Command::Underline2On);
					post_commands.insert(0, PrintData(PrintData::Command::Underline2Off));
				}
			}
		}
		else if(key == QLatin1String("lpt_textAlign") && el.tagName() != QLatin1String("p")) {
			QString val = attr.value();
			if(val == QLatin1String("right")) {
				pre_commands << PrintData(PrintData::Command::AlignRight);
				post_commands.insert(0, PrintData(PrintData::Command::AlignLeft));
			}
			else if(val == QLatin1String("center")) {
				pre_commands << PrintData(PrintData::Command::AlignCenter);
				post_commands.insert(0, PrintData(PrintData::Command::AlignLeft));
			}
			else if(val == QLatin1String("left")) {
				pre_commands << PrintData(PrintData::Command::AlignLeft);
				post_commands.insert(0, PrintData(PrintData::Command::AlignLeft));
			}
		}
		else if(key == QLatin1String("lpt_borderTop")) {
			QString val = attr.value();
			pre_commands << PrintData(PrintData::Command::HorizontalLine, val.toUtf8());
		}
		else if(key == QLatin1String("lpt_borderBottom")) {
			QString val = attr.value();
			post_commands << PrintData(PrintData::Command::HorizontalLine, val.toUtf8());
		}
	}
	print_context->line << pre_commands;
	if(el.tagName() == QLatin1String("p")) {
		QString ta = el.attribute(QLatin1String("lpt_textAlign"));
		Qt::Alignment text_align = Qt::AlignLeft;
		if(ta == QLatin1String("right"))
			text_align = Qt::AlignRight;
		else if(ta == QLatin1String("center"))
			text_align = Qt::AlignHCenter;
		QByteArray text = qf::core::Collator::toAscii7(QLocale::Czech, el.text(), false);
		print_context->line << PrintData(PrintData::Command::Text, text, text_width, text_align);
	}
	{
		for(QDomElement el1 = el.firstChildElement(); !el1.isNull(); ) {
			createPrinterData_helper(el1, print_context);
			el1 = el1.nextSiblingElement();
			if(!el1.isNull()) {
				//if(!is_halign || (is_halign && print_context->horizontalLayoutNestCount == 1)) {
				if(!is_halign) {
					print_context->line << PrintData(PrintData::Command::Eoln);
				}
			}
		}
	}
	print_context->line << post_commands;
	if(is_halign)
		print_context->horizontalLayoutNestCount--;
}

static QList<PrintLine> alignPrinterData(DirectPrintContext *print_context)
{
	QList<PrintLine> ret;
	PrintLine line;
	{
		line << PrintData(PrintData::Command::Init);
		ret.insert(ret.length(), line);
	}
	line.clear();
	for (int i = 0; i < print_context->line.count(); ++i) {
		const PrintData &pd = print_context->line[i];
		bool is_eol = (pd.command == PrintData::Command::Eoln) || (i == (print_context->line.count() - 1));
		if(pd.command != PrintData::Command::Eoln) {
			line << pd;
		}
		if(is_eol) {
			int fixed_text_len = 0;
			int spring_cnt = 0;
			for (int j = 0; j < line.length(); ++j) {
				const PrintData &pd2 = line[j];
				if(pd2.width < 0)
					spring_cnt++;
				else if(pd2.width > 0)
					fixed_text_len += pd2.width;
				else
					fixed_text_len += pd2.textLength();
			}
			for (int j = 0; j < line.length(); ++j) {
				PrintData &pd2 = line[j];
				if(pd2.isCommand())
					continue;
				int w = pd2.width;
				if(w < 0)
					w = (print_context->printerLineWidth - fixed_text_len) / spring_cnt;
				if(w > 0) {
					int w_rest = w - pd2.textLength();
					if(w_rest > 0) {
						if(pd2.alignment == Qt::AlignLeft)
							pd2.data = pd2.data + QByteArray(w_rest, ' ');
						else if(pd2.alignment == Qt::AlignRight)
							pd2.data = QByteArray(w_rest, ' ') + pd2.data;
						else if(pd2.alignment == Qt::AlignHCenter)
							pd2.data = QByteArray(w_rest/2+1, ' ') + pd2.data + QByteArray(w_rest/2+1, ' ');
					}
					pd2.data = pd2.data.mid(0, w);
					pd2.width = 0;
				}
			}
			ret.insert(ret.length(), line);
			line.clear();
		}
	}
	{
		line.clear();
		line << PrintData(PrintData::Command::Cut);
		ret.insert(ret.length(), line);
	}
	return ret;
}

static QList<QByteArray> interpretControlCodes(const QList<PrintLine> &lines, DirectPrintContext *ctx)
{
	QList<QByteArray> ret;
	const int cmd_length = sizeof(epson_commands) / sizeof(char*);
	for(const PrintLine line : lines) {
		QByteArray ba;
		int line_text_len = 0;
		for(const PrintData &pd : line) {
			if(pd.command == PrintData::Command::Text) {
				ba += pd.data;
				line_text_len += pd.data.length();
			}
			else if(pd.command == PrintData::Command::HorizontalLine) {
				QByteArray hr_data(ctx->printerLineWidth, pd.data[0]);
				if(line_text_len == 0) {
					ret.insert(ret.length(), ba + hr_data);
					ba.clear();
				}
				else {
					ret.insert(ret.length(), ba);
					ba = hr_data;
					line_text_len = hr_data.length();
				}
			}
			else {
				int ix = (int)pd.command;
				QF_ASSERT(ix < cmd_length, QString("%1 - Bad command index!").arg(ix), continue);
				ba += epson_commands[ix];
			}
		}
		ret.insert(ret.length(), ba);
	}
	return ret;
}

QList<QByteArray> ReceiptsPrinter::createPrinterData(const QDomElement &body, const ReceiptsPrinterOptions &printer_options)
{
	DirectPrintContext dpc;
	dpc.printerLineWidth = printer_options.characterPrinterLineLength();
	createPrinterData_helper(body, &dpc);
	/*
	{
		QByteArray ba;
		for(auto d : dpc.line)
			ba += d.toByteArray();
		qfInfo() << ba;
	}
	*/
	QList<PrintLine> lines = alignPrinterData(&dpc);
	for(auto l : lines) {
		QByteArray ba;
		for(auto d : l)
			ba += d.toByteArray();
		qfDebug() << ba;
	}
	QList<QByteArray> ret = interpretControlCodes(lines, &dpc);
	for(auto ba : ret) {
		qDebug() << ba;
	}
	return ret;
}

