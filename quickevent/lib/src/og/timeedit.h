#ifndef QUICKEVENT_OG_TIMEEDIT_H
#define QUICKEVENT_OG_TIMEEDIT_H

#include "../quickeventglobal.h"
#include "timems.h"

#include <qf/qmlwidgets/lineedit.h>

#include <qf/core/utils.h>

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT TimeEdit : public qf::qmlwidgets::LineEdit
{
	Q_OBJECT

	Q_PROPERTY(quickevent::og::TimeMs timeMs READ timeMs WRITE setTimeMs NOTIFY timeMsChanged USER true)
private:
	typedef qf::qmlwidgets::LineEdit Super;
public:
	TimeEdit(QWidget *parent = nullptr);
	~TimeEdit() Q_DECL_OVERRIDE;

	TimeMs timeMs() const;
	void setTimeMs(const TimeMs &t);
	Q_SIGNAL void timeMsChanged();
};

}}

#endif // QUICKEVENT_OG_TIMEEDIT_H
