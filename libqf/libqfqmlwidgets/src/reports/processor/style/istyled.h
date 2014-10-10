#ifndef QF_QMLWIDGETS_REPORTS_STYLE_ISTYLED_H
#define QF_QMLWIDGETS_REPORTS_STYLE_ISTYLED_H

class QObject;
class QString;
class QVariant;

namespace qf {
namespace qmlwidgets {
namespace reports {
class ReportItemReport;

class ReportProcessor;

namespace style {

class Sheet;

class IStyled
{
public:
    enum StyleGroup {SGOwn, SGColor, SGPen, SGBrush, SGFont, SGText};
public:
    IStyled(QObject *style_object, StyleGroup style_group);
public:
    QString name();
    virtual void setName(const QString &s);
protected:
	ReportItemReport* reportItem();
	ReportProcessor* reportProcessor();
    Sheet* rootStyleSheet();

    QObject* styleobjectFromVariant(const QVariant &v, StyleGroup style_group = SGOwn, bool should_exist = true);

    QString nextSequentialName();

    void setDirty(bool b = true) {m_dirty = b;}
    bool isDirty() {return m_dirty;}
private:
	QObject *m_styleObject;
    StyleGroup m_styleGroup;
    bool m_dirty;
};

}}}}

#endif
