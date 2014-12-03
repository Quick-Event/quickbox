#include "reportitempara.h"

#include "reportprocessor.h"
#include "reportpainter.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//==========================================================
//                                    ReportItemPara
//==========================================================
ReportItemPara::ReportItemPara(ReportItem * parent)
	: ReportItemFrame(parent)
{
	qfLogFuncFrame();
	//qfInfo() << el.text();
}

void ReportItemPara::resetIndexToPrintRecursively(bool including_para_texts)
{
	if(including_para_texts)
		indexToPrint = 0;
}

ReportItem::PrintResult ReportItemPara::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame();
	return Super::printMetaPaint(out, bounding_rect);
}

ReportItem::PrintResult ReportItemPara::printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this << bounding_rect.toString();
	PrintResult res = PrintOk;
	if(indexToPrint == 0) {
		printedText = paraText();
	}
	QString text = printedText.mid(indexToPrint);
	int initial_index_to_print = indexToPrint;

	QString sql_id = sqlId();
	/// tiskne se prazdny text
	bool omit_empty_text = isOmitEmptyText();
	if(text.isEmpty() && omit_empty_text) {
	}
	else {
		QString text_to_layout = text;
		//qfWarning() << "length: " << text.length() << " text: [" << text << "]\n" << text.toUtf8().toHex();
		bool text_item_should_be_created = true;
		style::CompiledTextStyle style;
		style::Text *p_text_style = effectiveTextStyle();
		if(p_text_style) {
			style = p_text_style->textStyle();
		}
		/*--
		{
			QString s;
			s = elementAttribute("font");
			if(!!s) style.font = processor()->context().styleCache().font(s);
			s = elementAttribute("pen");
			if(!!s) style.pen = processor()->context().styleCache().pen(s);
			//QBrush brush = processor()->context().brushFromString(element.attribute("brush"));
			qfDebug() << "\tfont:" << style.font.toString();
			//qfDebug() << "\tpen color:" << pen.color().name();
			//qfDebug() << "\tbrush color:" << brush.color().name();
		}
		--*/
		QFontMetricsF font_metrics = processor()->fontMetrics(style.font());
		QTextOption text_option;
		{
			if(isTextWrap())
				text_option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
			//alignment_flags |= Qt::TextWordWrap;
			int al = textHAlign() | textVAlign();
			Qt::Alignment alignment_flags = (Qt::Alignment)al;
			text_option.setAlignment(alignment_flags);
		}
		Rect br;
		/// velikost boundingRect je v mm, tak to prepocitej na body vystupniho zarizeni
		br = qmlwidgets::graphics::mm2device(bounding_rect, processor()->paintDevice());

		bool render_check_mark = false;
		QRegExp rx = ReportItemMetaPaint::checkReportSubstitutionRegExp;
		if(rx.exactMatch(text_to_layout)) {
			//bool check_on = rx.capturedTexts().value(1) == "1";
			br = font_metrics.boundingRect('X');
			render_check_mark = true;
			indexToPrint += text_to_layout.length();
		}
		else {
			if(text_to_layout.isEmpty()) {
				/// neni omitEmptyString, takze i prazdnej text vyrendruj alespon jako mezeru aby se na to dalo treba kliknout
				text_to_layout = ' ';
			}

			//text.replace(ReportItemMetaPaint::checkOnReportSubstitution, "X");
			//text.replace(ReportItemMetaPaint::checkOffReportSubstitution, "X");
			//qfInfo().noSpace().color(QFLog::Green) << "index to print: " << indexToPrint << " text: '" << text << "'";
			//qfInfo() << "bounding rect:" << bounding_rect.toString();
			//qfWarning() << "device physical DPI:" << processor()->paintDevice()->physicalDpiX() << processor()->paintDevice()->physicalDpiY();
			//qfWarning().noSpace() << "'" << text << "' font metrics: " << br.toString();

			//QString text = element.text().simplified().replace("\\n", "\n");
			//qfInfo() << "br:" << br.toString();
			//Rect br_debug = br;
			//bool splitted = false;
			/// do layout
			{
				qreal leading = font_metrics.leading();
				qreal height = 0;
				qreal width = 0;
				textLayout.setFont(style.font());
				//Qt::Alignment alignment = (~Qt::Alignment()) & flags;
				//QTextOption opt(alignment);
				//opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
				textLayout.setTextOption(text_option);
				textLayout.setText(text_to_layout);
				textLayout.beginLayout();
				//bool rubber_frame = designedRect.isRubber(LayoutVertical);
				//int old_pos = 0;
				//QString tx1 = text;
				//qfInfo() << "text to layout:" << text;
				//int line_cnt = 0;
				bool finished = false;
				while (!finished) {
					QTextLine line = textLayout.createLine();
					finished = !line.isValid();
					if(!finished) {
						/*
						if(!finished && line_cnt > 0) {
							qfInfo().noSpace() << "LINE ##: " << line.textStart() << '[' << text.mid(line.textStart(), line.textLength()) << "] + " << line.textLength() << " of " << text.length();
							qfInfo() << "finished:" << finished << "(line.textLength() == 0):" << (line.textLength() == 0) << "(line.textStart() + line.textLength() == text.length()):" << (line.textStart() + line.textLength() == text.length());
						}
						*/
						//line_cnt++;
						//old_pos = line.textStart();
						//qfInfo() << "setting line width to:" << br.width();
						line.setLineWidth(br.width()); /// setWidth() nastavi spravne line.height(), proto musi byt pred merenim popsane vysky.
						//qfInfo() << "text rest:" << text_to_layout.mid(line.textStart());

						if((line.textLength() == 0) && (line.textStart() + line.textLength() == text_to_layout.length())) {
							/// nevim kde je chyba, pri vicerakovych textech mi to pridava jeden prazdnej radek na konec, takhle se tomu snazim zabranit (Qt 4.6.3)
							finished = true;
						}
						else {
							qreal interline_space = (height > 0)? leading: 0;
							if(height + interline_space + line.height() > br.height()) {
								//qfInfo() << "NEEEEEEEE veslo se";
								res = PrintNotFit;
								if(height == 0) {
									/// nevejde se ani jeden radek
									text_item_should_be_created = false;
									break;
								}
								else {
									/// neco se preci jenom veslo
									//splitted = true;
									//qfInfo() << "\tbounding_rect rect:" << bounding_rect.toString();
									//qfInfo() << "\tbr:" << br.toString();
									//qfInfo() << "\theight:" << height;
									int pos = line.textStart();
									indexToPrint += pos;
									//qfInfo() << "POS:" << pos << "index toprint:" << indexToPrint;
									//qfInfo() << text.mid(0, pos).simplified();
									//text_to_layout = text_to_layout.left(pos);
									//res = PrintOk;
									//res.flags = ReportItem::FlagPrintAgain;
									break;
								}
								//line.setLineWidth(123456789); /// vytiskni to az do konce
							}
							height += interline_space;
							//if(line_cnt > 1) qfInfo().noSpace() << "LINE ##: " << line.textStart() << '[' << text.mid(line.textStart(), line.textLength()) << "] + " << line.textLength() << " of " << text.length();
							line.setPosition(QPointF(0., height));
							height += line.height();
							width = qMax(width, line.naturalTextWidth());
						}
					}
					if(finished) {
						//qfInfo() << "veslo se VSECHNO";
						indexToPrint = printedText.length();
						//break;
					}
				}
				textLayout.endLayout();
				br.setWidth(width);
				br.setHeight(height);
				// musim to takhle premerit, jina
				//br = font_metrics.boundingRect(br, flags, text);
				//br = font_metrics.boundingRect(br_debug, 0, text);
				//qfInfo() << "\tbr2:" << br.toString();
			}
		}
		/*
		int x_dpi = processor()->paintDevice()->logicalDpiX();
		int y_dpi = processor()->paintDevice()->logicalDpiY();
		br.setWidth(br.width() * 25.4 / x_dpi);
		br.setHeight(br.height() * 25.4 / y_dpi);
		*/
		/// velikost boundingRect je v bodech vystupniho zarizeni, tak to prepocitej na mm
		br = qmlwidgets::graphics::device2mm(br, processor()->paintDevice());
		/// posun to na zacatek, alignment ramecku to zase vrati
		br.moveTopLeft(bounding_rect.topLeft());
		//qfInfo().noSpace() << "text: '" << text << "'";
		if(text_item_should_be_created ) {
			ReportItemMetaPaintText *mt;
			if(render_check_mark )
				mt = new ReportItemMetaPaintCheck(out, this);
			else {
				mt = new ReportItemMetaPaintText(out, this);
				mt->sqlId = sql_id;
				//--mt->editGrants = elementAttribute("editGrants");
			}
			//qfInfo() << "creating item:" << mt;
			mt->pen = style.pen();
			mt->font = style.font();
			mt->text = text.mid(0, indexToPrint - initial_index_to_print);
			//qfWarning() << "text:" << text;
			mt->textOption = text_option;
			mt->renderedRect = br;
			mt->renderedRect.flags = designedRect.flags;
		}
		//qfDebug().color(QFLog::Green, QFLog::Red) << "\tleading:" << processor()->fontMetrics(style.font).leading() << "\theight:" << processor()->fontMetrics(style.font).height();
		qfDebug() << "\tchild rendered rect:" << br.toString();
	}
	qfDebug() << "\t<<< CHILDREN paraText return:" << res.toString();
	//res = checkPrintResult(res);
	return res;
}

QString ReportItemPara::paraText()
{
	qfLogFuncFrame();
	QString ret;
	if(m_getTextJsFn.isCallable()) {
		QJSValue jsv = m_getTextJsFn.call();
		ret = jsv.toString();
	}
	else if(m_getTextCppFn) {
		ret = m_getTextCppFn();
	}
	else {
		ret = text();
	}
	{
		static QString new_line;
		if(new_line.isEmpty())
			new_line += QChar::LineSeparator;
		ret.replace("\\n", new_line);
		ret.replace("\n", new_line);

		/// jinak nedokazu zadat mezeru mezi dvema <data> elementy nez <data>\s<data>
		ret.replace("\\s", " ");
		/// non breaking space
		ret.replace("\\S", QString(QChar::Nbsp));
	}
	return ret;
}

