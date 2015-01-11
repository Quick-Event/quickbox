#ifndef OGTIMEEDIT_H
#define OGTIMEEDIT_H

#include <qf/qmlwidgets/lineedit.h>

#include <qf/core/utils.h>

class OGTimeEdit : public qf::qmlwidgets::LineEdit
{
	Q_OBJECT

	Q_PROPERTY(int obTimeMs READ obTimeMs WRITE setObTimeMs NOTIFY obTimeMsChanged)
private:
	typedef qf::qmlwidgets::LineEdit Super;
public:
	OGTimeEdit(QWidget *parent = nullptr);
	~OGTimeEdit() Q_DECL_OVERRIDE;

	int obTimeMs() const;
	void setObTimeMs(int msec);
	Q_SIGNAL void obTimeMsChanged(int value);
};

#endif // OGTIMEEDIT_H
