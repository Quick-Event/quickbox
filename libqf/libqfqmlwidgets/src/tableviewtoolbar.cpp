#include "tableviewtoolbar.h"
#include "tableview.h"
#include "action.h"
#include "style.h"

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QKeyEvent>

//namespace qfc = qf::core;
//namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets;

class FilterCombo : public QComboBox
{
	Q_OBJECT
private:
	typedef QComboBox Super;
public:
	FilterCombo(QWidget *parent = nullptr) : QComboBox(parent) {}
	~FilterCombo() Q_DECL_OVERRIDE {}

	Q_SIGNAL void filterFocusReleased();
protected:
	void keyReleaseEvent(QKeyEvent *ev) Q_DECL_OVERRIDE;
};

void FilterCombo::keyReleaseEvent(QKeyEvent *ev)
{
	qfLogFuncFrame() << ev->key() << (ev->key() == Qt::Key_Escape);
	if(ev->key() != Qt::Key_Escape) {
		Super::keyReleaseEvent(ev);
	}
	else {
		emit filterFocusReleased();
	}
}

TableViewToolBar::TableViewToolBar(QWidget *parent) :
	QToolBar(parent)
{
	m_filterCombo = new FilterCombo();
	//m_filterCombo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_filterCombo->setMinimumWidth(fontMetrics().width('X') * 15);
	m_filterCombo->setEditable(true);
	m_filterCombo->lineEdit()->setClearButtonEnabled(true);
	connect(m_filterCombo, &QComboBox::editTextChanged, this, &TableViewToolBar::emitFilterStringChanged);
	connect(m_filterCombo, SIGNAL(activated(QString)), this, SLOT(emitFilterStringChanged(QString)));
}

TableViewToolBar::~TableViewToolBar()
{
}

void TableViewToolBar::setTableView(TableView *table_view)
{
	qfLogFuncFrame() << m_filterCombo;
	if(table_view) {
		m_pendingActions = table_view->toolBarActions();
		/// cannot add actions here from QML context because of bug in Qt5.3.1
		QMetaObject::invokeMethod(this, "addPendingActions", Qt::QueuedConnection);
		connect(this, &TableViewToolBar::filterStringChanged, table_view, &TableView::filterByString);
		connect(table_view, &TableView::filterDialogRequest, this, &TableViewToolBar::onFilterDialogRequest);
		if(m_filterCombo) {
			connect(m_filterCombo, SIGNAL(filterFocusReleased()), table_view, SLOT(setFocus()));
		}
	}
}

void TableViewToolBar::addPendingActions()
{
	QList<QAction*> lst;
	Q_FOREACH(auto a, m_pendingActions)
		lst << a;
	addActions(lst);
	QLabel *lbl = new QLabel(tr("Filter"));
	auto *style = Style::instance();
	QPixmap px = style->pixmapFromSvg("find");
	lbl->setPixmap(px);
	addWidget(lbl);
	addWidget(m_filterCombo);
}

void TableViewToolBar::emitFilterStringChanged(const QString &s)
{
	qfLogFuncFrame() << s;
	emit filterStringChanged(s);
	m_filterCombo->setFocus();
}

void TableViewToolBar::onFilterDialogRequest()
{
	m_filterCombo->lineEdit()->selectAll();
	m_filterCombo->setFocus();
}

#include "tableviewtoolbar.moc"
