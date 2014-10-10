#include "istyled.h"
#include "../reportitem.h"

#include <qf/core/log.h>

#include <QObject>

using namespace qf::qmlwidgets::reports::style;

IStyled::IStyled(QObject *style_object, StyleGroup style_group)
    : m_styleObject(style_object), m_styleGroup(style_group)
{
    m_dirty = true;
}

QString IStyled::name()
{
    QF_ASSERT(m_styleObject != nullptr, "Drived object is NULL", return QString());
    return m_styleObject->objectName();
}

void IStyled::setName(const QString &s)
{
    QF_ASSERT(m_styleObject != nullptr, "Driven object is NULL", return);
	Sheet *root_ss = rootStyleSheet();
	if(root_ss) {
		rootStyleSheet()->setStyleObjectForName(m_styleGroup, m_styleObject->objectName(), nullptr);
	    m_styleObject->setObjectName(s);
	    rootStyleSheet()->setStyleObjectForName(m_styleGroup, m_styleObject->objectName(), m_styleObject);
	}
}

qf::qmlwidgets::reports::ReportItemReport *IStyled::reportItem()
{
	ReportItemReport *ret = nullptr;
	QObject *it = m_styleObject;
	while(it) {
		ret = qobject_cast<ReportItemReport*>(it);
		if(ret) {
			break;
		}
		it = it->QObject::parent();
	}
	//if(ret == nullptr)
	//	qfError() << "FCK";
	QF_ASSERT(ret != nullptr, "Cannot find Report Root Item", return ret);
	return ret;
}

qf::qmlwidgets::reports::ReportProcessor *IStyled::reportProcessor()
{
	ReportProcessor *ret = nullptr;
	ReportItemReport *rir = reportItem();
	if(rir) {
		ret = rir->reportProcessor();
	}
	QF_ASSERT(ret != nullptr, "Report Style Item without ReportProcessor", return ret);
	return ret;
}

Sheet *IStyled::rootStyleSheet()
{
	Sheet *ret = nullptr;
	ReportItemReport *rir = reportItem();
	if(rir)
		ret = rir->styleSheet();
    return ret;
}

QObject *IStyled::styleobjectFromVariant(const QVariant &v, StyleGroup style_group, bool should_exist)
{
    QObject *ret = nullptr;
    if(v.isValid()) {
        ret = v.value<QObject*>();
        if(!ret) {
            if(v.type() == QVariant::String) {
                StyleGroup sg = style_group;
                if(sg == SGOwn)
                    sg = m_styleGroup;
                ret = rootStyleSheet()->styleObjectForName(sg, v.toString(), should_exist);
            }
        }
    }
    return ret;
}

QString IStyled::nextSequentialName()
{
    static int n = 0;
    return QString("%1_%2").arg(m_styleGroup).arg(++n);
}
