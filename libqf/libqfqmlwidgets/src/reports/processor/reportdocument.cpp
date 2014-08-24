#include "reportdocument.h"

#include <qf/core/log.h>
#include <qf/core/string.h>
#include <qf/core/exception.h>
#include <qf/core/utils/searchdirs.h>
#include <qf/core/utils/fileutils.h>

#include <QPair>
#include <QFile>
#include <QDir>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

QString ReportDocument::includedFromAttributeName = "_includedFrom";
QString ReportDocument::includeSrcAttributeName = "_includeSrc";

ReportDocument::ReportDocument() :
	QDomDocument()
{
}

ReportDocument::ReportDocument(const QString &root_name) :
	QDomDocument()
{
	QByteArray ba = "<?xml version='1.0' encoding='UTF-8'?><" + root_name.toLatin1() + "/>";
	QDomDocument::setContent(ba);
}

bool ReportDocument::setContent(const QByteArray &ba)
{
	qfLogFuncFrame();
	if(ba.isEmpty()) {
		*this = ReportDocument();
		return true;
	}
	QString err_msg;
	int err_line, err_col;
	if(!QDomDocument::setContent(ba, false, &err_msg, &err_line, &err_col)) {
		err_msg = trUtf8("ERROR", "ReportDocument::setContent")
		+ "\n\n"
		+ trUtf8("line: %1 ", "ReportDocument::setContent").arg(err_line)
		+ trUtf8("col: %1", "ReportDocument::setContent").arg(err_col)
		+ "\n\n"
		+ err_msg;
		qfError() << err_msg;
		return false;
	}
	return true;
}

bool ReportDocument::setContentAndResolveIncludes(const QString &file_path, qfu::SearchDirs *search_dirs)
{
	qfLogFuncFrame() << file_path;
	bool ret = false;
	QPair<QString, QString> abs_file;
	if(search_dirs)
		abs_file = search_dirs->findFile2(file_path);
	else
		abs_file.second = file_path;
	QString abs_file_path = qfu::FileUtils::joinPath(abs_file.first, abs_file.second);
	if(abs_file_path.isEmpty()) {
		qfError() << tr("File '%1' can not be resolved.").arg(file_path);
		return false;
	}
	else {
		QByteArray ba;
		if(search_dirs) ba = search_dirs->loadFile(abs_file_path);
		else {
			QFile f(abs_file_path);
			if(f.open(QFile::ReadOnly))
				ba = f.readAll();
			else {
				qfError() << tr("Can't open file '%1' for reading.").arg(abs_file_path);
			}
		}
		ret = setContent(ba);
		if(ret) {
			setFileName(abs_file.second); /// pokud je dokument oteviran s relativni cestou, musi byt i zde cesta relativni, jinak nebude fungovat resolveIncludes()
			resolveIncludes(search_dirs);
		}
	}
	return ret;
}

void ReportDocument::resolveIncludes_helper(const QDomNode &_parent_nd, core::utils::SearchDirs *search_dirs)
{
	//qfLogFuncFrame();
	QDomNode nd_parent = _parent_nd;
	while(1) {
		bool include_found = false;
		for(QDomElement el=nd_parent.firstChildElement(); !el.isNull(); ) {
			//qfDebug() << "\ttag:" << el.tagName();
			if(el.tagName() == "include") {
				//qfDebug() << QF_FUNC_NAME << "\tcurrent element path:" << el.path();
				include_found = true;
				QString include_src = el.attribute("src").trimmed();
				if(include_src.isEmpty())
					include_src = el.attribute("href").trimmed();
				if(include_src.isEmpty()) {
					qfError() << "Empty or missing 'src' or 'href' attribute in <include> element.";
					return;
				}
				QString absolute_file_name = include_src;
				qfc::String include_resolved_file = include_src;
				//qfDebug() << "\tsrc:" << include_src;
				//qfDebug() << "\tfileName:" << fileName();
				//qfDebug() << "\tqfu::FileUtils::path(fileName()):" << qfu::FileUtils::path(fileName());
				//qfDebug() << "\tqfu::FileUtils::joinPath(qfu::FileUtils::path(fileName()), s):" << qfu::FileUtils::joinPath(qfu::FileUtils::path(fileName()), s);
				qfDebug() << "\t included file src:" << include_src;
				bool global_include = false;
				if(include_resolved_file.value(0) == '[' && include_resolved_file.value(-1) == ']') { /// nemuzu pouzit <> jako v C, protoze XML atribut nesmi obsahovat znaky '<' a '&'
					/// hleda se relativne k searchDirs
					include_resolved_file = include_resolved_file.slice(1, -1);
					global_include = true;
				}
				if(QDir::isRelativePath(include_resolved_file)) {
					if(global_include) {
						/// hleda se relativne k searchDirs
						QPair<QString, QString> abs_file;
						if(search_dirs) abs_file = search_dirs->findFile2(include_resolved_file);
						else abs_file.second = include_resolved_file;
						QString absolute_file_name = qfu::FileUtils::joinPath(abs_file.first, abs_file.second);
						include_resolved_file = abs_file.second;
						//absolute_file_name = search_dirs.findFile(file_name);
					}
					else {
						/// zjisti z jakeho souboru je nainkludovan soucasny dokument
						QString included_from_file = /*recent_include_file(el);
						if(included_from_file.isEmpty()) included_from_file =*/ fileName();
						qfDebug() << "\t included_from_file:" << included_from_file;
						QString included_from_path = qfu::FileUtils::path(included_from_file);
						qfDebug() << "\t included_from_path:" << included_from_path;
						qfDebug() << "\t searchng for file:" << qfu::FileUtils::joinPath(included_from_path, include_resolved_file) << "(" << included_from_path << "join" << include_resolved_file << ")";
						include_resolved_file = qfu::FileUtils::joinPath(included_from_path, include_resolved_file);
						if(QDir::isRelativePath(include_resolved_file)) {
							/// pokud je soubor inkludovan ze souboru s relativni cestou, hledej ho na vsech cestach (takhle si delam unionfs pro reporty)
							QPair<QString, QString> abs_file;
							if(search_dirs) abs_file = search_dirs->findFile2(include_resolved_file);
							else abs_file.second = include_resolved_file;
							absolute_file_name = qfu::FileUtils::joinPath(abs_file.first, abs_file.second);
							include_resolved_file = abs_file.second;
							qfDebug() << "\t relative path:" << abs_file.first << "+" << abs_file.second;
							//absolute_file_name = search_dirs.findFile(file_name);
						}
						else {
							//qfDebug() << "\t absolute path";
							absolute_file_name = include_resolved_file;
						}
					}
				}
				else {
					absolute_file_name = include_resolved_file;
				}
				if(absolute_file_name.isEmpty()) {
					qfError() << tr("File '%1' can not be resolved.").arg(include_src);
					return;
				}
				//QFile f(absolute_file_name);
				qfDebug() << "\t absolute file name:" << absolute_file_name;
				qfDebug() << "\t include_resolved_file:" << include_resolved_file;
				ReportDocument doc;
				doc.setContentAndResolveIncludes(include_resolved_file, search_dirs);
				QDomElement included_doc_root_el = doc.firstChildElement();
				//qfDebug() << "\tfirst child:" << el1.tagName();
				//QDomDocumentFragment frag;
				QDomElement el_next = el.nextSiblingElement();
				for(QDomElement imported_el = included_doc_root_el.firstChildElement(); !imported_el.isNull(); imported_el = imported_el.nextSiblingElement()) {
					QDomElement el_to_insert = importNode(imported_el, true).toElement();
					//qfDomWalk(el_to_insert, print_owner, 0);
					if(!el_to_insert.isNull()) {
						el_to_insert.setAttribute(ReportDocument::includedFromAttributeName, include_resolved_file);
						el_to_insert.setAttribute(ReportDocument::includeSrcAttributeName, include_src);
						qfDebug() << "\t including element:" << el_to_insert.tagName() << "from:" << el_to_insert.attribute(ReportDocument::includedFromAttributeName);
					}
					//resolveIncludesHook(el, nd_to_insert, doc);
					/// vloz pred element <include ... >
					nd_parent.insertBefore(el_to_insert, el);
				}
				/// vymaz element <include ... >, protoze misto neho uz je obsah inkludovaneho souboru
				nd_parent.removeChild(el);
				el = el_next;
				//qfInfo() << this->toString();
			}
			else {
				resolveIncludes_helper(el, search_dirs);
				el=el.nextSiblingElement();
			}
		}
		if(!include_found) break;
	}
}

void ReportDocument::resolveIncludes(qfu::SearchDirs *search_dirs)
{
	qfDebug() << QF_FUNC_NAME;
	resolveIncludes_helper(*this, search_dirs);
	//qfDebug() << toString();
}

QDomElement ReportDocument::elementFromString(QDomDocument doc, const QString &s)
{
	QDomDocument doc1;
	if(!doc1.setContent(s)) {
		qfError() << "Error seting element content";
		return QDomElement();
	}
	QDomNode nd = doc.importNode(doc1.documentElement(), true);
	QDomElement el = nd.toElement();
	return el;
}

void ReportDocument::unresolveIncudes_helper(const QDomElement &_el, QList<ReportDocument> &list)
{
	QDomElement el_parent = _el;
	QMap<QString, QList<QDomElement> > included_elements;
	QString current_include_src;
	for(QDomElement el=el_parent.firstChildElement(); !el.isNull(); el = el.nextSiblingElement()) {
		QString s = el.attribute(ReportDocument::includedFromAttributeName);
		if(!s.isEmpty()) {
			QString include_src = el.attribute(ReportDocument::includeSrcAttributeName);
			if(!include_src.isEmpty() && include_src != current_include_src) {
				QDomElement el_inc = createElement("include");
				el_inc.setAttribute("src", include_src);
				el_parent.insertBefore(el_inc, el);
				current_include_src = include_src;
			}
			included_elements[s] << el;
			//el_parent.removeChild(el);
		}
		unresolveIncudes_helper(el, list);
	}
	QMap< QString, QDomDocument > included_documents;
	foreach(QString fn, included_elements.keys()) {
		ReportDocument doc("inc");
		doc.setFileName(fn);
		QDomElement doc_root_el = doc.documentElement();
		QList<QDomElement> lst = included_elements.value(fn);
		foreach(QDomElement el, lst) {
			QDomNode nd = el_parent.removeChild(el);
			nd = doc.importNode(nd, true);
			el = nd.toElement();
			el.removeAttribute(ReportDocument::includeSrcAttributeName);
			el.removeAttribute(ReportDocument::includedFromAttributeName);
			doc_root_el.appendChild(el);
		}
		list << doc;
		//included_documents[fn] = doc;
	}
}

QList<ReportDocument> ReportDocument::unresolveIncudes()
{
	QList<ReportDocument > ret;
	ReportDocument doc = clone();
	ret << doc;
	unresolveIncudes_helper(doc.documentElement(), ret);
	return ret;
}

ReportDocument ReportDocument::clone() const
{
	qfLogFuncFrame();
	ReportDocument doc;
	for(QDomNode nd=firstChild(); !nd.isNull(); nd=nd.nextSibling()) {
		QDomNode nd2 = doc.importNode(nd, true);
		if(!nd2.isNull())
			doc.appendChild(nd2);
		else
			qfError() << "imported node is NULL";
	}
	return doc;
}
