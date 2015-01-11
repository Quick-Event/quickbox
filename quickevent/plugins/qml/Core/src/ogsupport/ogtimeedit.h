#ifndef OGTIMEEDIT_H
#define OGTIMEEDIT_H

#include "ogtimems.h"

#include <qf/qmlwidgets/lineedit.h>

#include <qf/core/utils.h>

class OGTimeEdit : public qf::qmlwidgets::LineEdit
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

#endif // OGTIMEEDIT_H
