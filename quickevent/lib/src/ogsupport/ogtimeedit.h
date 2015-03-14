#ifndef QUICKEVENT_OGTIMEEDIT_H
#define QUICKEVENT_OGTIMEEDIT_H

#include "../quickeventglobal.h"
#include "ogtimems.h"

#include <qf/qmlwidgets/lineedit.h>

#include <qf/core/utils.h>

class QUICKEVENT_DECL_EXPORT OGTimeEdit : public qf::qmlwidgets::LineEdit
{
	Q_OBJECT

	Q_PROPERTY(OGTimeMs ogTimeMs READ ogTimeMs WRITE setOGTimeMs NOTIFY ogTimeMsChanged USER true)
private:
	typedef qf::qmlwidgets::LineEdit Super;
public:
	OGTimeEdit(QWidget *parent = nullptr);
	~OGTimeEdit() Q_DECL_OVERRIDE;

	OGTimeMs ogTimeMs() const;
	void setOGTimeMs(const OGTimeMs &t);
	Q_SIGNAL void ogTimeMsChanged();
};

#endif // QUICKEVENT_OGTIMEEDIT_H
