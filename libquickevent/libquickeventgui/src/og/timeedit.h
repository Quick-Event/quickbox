#ifndef QUICKEVENTGUI_OG_TIMEEDIT_H
#define QUICKEVENTGUI_OG_TIMEEDIT_H

#include "../quickeventguiglobal.h"

#include <quickevent/core/og/timems.h>

#include <qf/qmlwidgets/lineedit.h>

#include <qf/core/utils.h>

namespace quickevent {
namespace gui {
namespace og {

class QUICKEVENTGUI_DECL_EXPORT TimeEdit : public qf::qmlwidgets::LineEdit
{
	Q_OBJECT

	Q_PROPERTY(quickevent::core::og::LapTimeMs timeMs READ timeMs WRITE setTimeMs NOTIFY timeMsChanged USER true)
private:
	typedef qf::qmlwidgets::LineEdit Super;
public:
	TimeEdit(QWidget *parent = nullptr);
	~TimeEdit() Q_DECL_OVERRIDE;

	core::og::LapTimeMs timeMs() const;
	void setTimeMs(const core::og::LapTimeMs &t);
	Q_SIGNAL void timeMsChanged();
};

}}}

#endif // QUICKEVENTGUI_OG_TIMEEDIT_H
