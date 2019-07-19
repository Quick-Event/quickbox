#ifndef QF_QMLWIDGETS_PROGRESSBAR_H
#define QF_QMLWIDGETS_PROGRESSBAR_H

#include "qmlwidgetsglobal.h"

#include <QProgressBar>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT ProgressBar : public QProgressBar
{
	Q_OBJECT
private:
	typedef QProgressBar Super;
public:
	explicit ProgressBar(QWidget *parent = nullptr);
};

}}

#endif // PROGRESSBAR_H
