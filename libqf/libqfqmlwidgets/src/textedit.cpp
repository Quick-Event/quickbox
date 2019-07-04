#include "textedit.h"

#include <qf/core/log.h>

namespace qf {
namespace qmlwidgets {

TextEdit::TextEdit(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &TextEdit::textChanged, this, &TextEdit::onTextChanged);
}

QVariant TextEdit::dataValue()
{
	return toPlainText();
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

void TextEdit::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << dataId() << val;
	QString new_text = val.isNull()? QString(): val.toString();
	if(checkSetDataValueFirstTime()) {
		setPlainText(new_text);
	}
	else {
		if(new_text != toPlainText()) {
			BlockerScope bs(m_blockTextChanged);
			setPlainText(new_text);
			saveDataValue();
			emit dataValueChanged(new_text);
		}
	}
}

void TextEdit::onTextChanged()
{
	if(m_blockTextChanged)
		return;
	saveDataValue();
	emit dataValueChanged(toPlainText());
}

} // namespace qmlwidgets
} // namespace qf
