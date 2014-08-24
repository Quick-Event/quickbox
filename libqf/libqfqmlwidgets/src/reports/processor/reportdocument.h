#ifndef QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H
#define QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H

#include <qf/core/exception.h>

#include <QDomDocument>
#include <QCoreApplication>

namespace qf {

namespace core {
namespace utils {
class SearchDirs;
}
}

namespace qmlwidgets {
namespace reports {

class ReportDocument : public QDomDocument
{
	Q_DECLARE_TR_FUNCTIONS(qf::qmlwidgets::reports::ReportDocument)
public:
	explicit ReportDocument();
	explicit ReportDocument(const QString &root_name);
public:
	static QString includedFromAttributeName;
	static QString includeSrcAttributeName;
public:
	QString fileName() const {return m_fileName;}
	void setFileName(const QString &fname) {m_fileName = fname;}
	bool setContent(const QByteArray& ba);
	bool setContentAndResolveIncludes(const QString& file_path, qf::core::utils::SearchDirs *search_dirs);

	//! Nahradi vsechny include elementy v dokumentu jejich obsahem.
	void resolveIncludes(qf::core::utils::SearchDirs *search_dirs);
	/// najde vsechny inkludovany soubory a vrati je v listu
	/// 1. prvek listu je hlavni dokument
	QList<ReportDocument > unresolveIncudes();

	ReportDocument clone() const;

	QDomElement elementFromString(const QString &s);
	static QDomElement elementFromString(QDomDocument doc, const QString &s);
protected:
	void resolveIncludes_helper(const QDomNode &nd, qf::core::utils::SearchDirs *search_dirs);
	void unresolveIncudes_helper(const QDomElement &_el, QList<ReportDocument > &list);
protected:
	QString m_fileName;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTDOCUMENT_H
