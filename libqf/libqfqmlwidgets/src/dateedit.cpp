#include "dateedit.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

DateEdit::DateEdit(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &QDateEdit::dateChanged, this, &DateEdit::onDateChanged);
}

QVariant DateEdit::dataValue()
{
	return date();
}

namespace {
class BlockerScope
{
public:
	explicit BlockerScope(bool &blocker_var) : m_blockerVar(blocker_var) {m_blockerVar = true;}
	~BlockerScope() {m_blockerVar = false;}
private:
	bool &m_blockerVar;
};
}

void DateEdit::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << "new:" << val << "old:" << date();
	QDate new_date = val.toDate();
	if(checkSetDataValueFirstTime()) {
		setDate(new_date);
	}
	else {
		if(new_date != date()) {
			BlockerScope bs(m_blockDateChanged);
			setDate(new_date);
			saveDataValue();
			emit dataValueChanged(new_date);
		}
	}
}

void DateEdit::onDateChanged(const QDate &new_date)
{
	if(m_blockDateChanged)
		return;
	qfLogFuncFrame() << new_date;
	saveDataValue();
	emit dataValueChanged(new_date);
}
