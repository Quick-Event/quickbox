#include "drawingganttwidget.h"
#include "ui_drawingganttwidget.h"

#include "classitem.h"
#include "ganttitem.h"
#include "ganttscene.h"

#include <qf/qmlwidgets/style.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <QLineEdit>
#include <QMessageBox>
#include <QCheckBox>

using namespace drawing;

DrawingGanttWidget::DrawingGanttWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::DrawingGanttWidget)
{
	setTitle(tr("Draw tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);

	ui->actSave->setIcon(qf::qmlwidgets::Style::instance()->icon("save"));
	ui->actFind->setIcon(qf::qmlwidgets::Style::instance()->icon("find"));

	m_ganttScene = new GanttScene(this);
	ui->ganttView->setScene(m_ganttScene);
}

DrawingGanttWidget::~DrawingGanttWidget()
{
	delete ui;
}

void DrawingGanttWidget::settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg)
{
	qf::qmlwidgets::ToolBar *tb = dlg->toolBar("main", true);
	tb->addAction(ui->actSave);
	m_edFind = new QLineEdit();
	m_edFind->setMaximumWidth(QFontMetrics(font()).horizontalAdvance('X') * 8);
	connect(m_edFind, &QLineEdit::textEdited, this, &DrawingGanttWidget::on_actFind_triggered);
	tb->addWidget(m_edFind);
	tb->addAction(ui->actFind);

	auto *cb_check_runners = new QCheckBox(tr("Runners clash"));
	auto *cb_check_courses = new QCheckBox(tr("Courses clash"));
	auto update_class_check = [this, cb_check_runners, cb_check_courses]() {
		QSet<ClassItem::ClashType> checks;
		if (cb_check_runners->isChecked()) {
			checks << ClassItem::ClashType::RunnersOverlap;
		}
		if (cb_check_courses->isChecked()) {
			checks << ClassItem::ClashType::CourseOverlap;
		}
		m_ganttScene->ganttItem()->setClashTypesToCheck(checks);
	};
	{
		auto *cb = cb_check_runners;
		cb->setChecked(true);
		tb->addWidget(cb);
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
		connect(cb, &QCheckBox::stateChanged, this, update_class_check);
#else
		connect(cb, &QCheckBox::checkStateChanged, this, update_class_check);
#endif
	}
	{
		auto *cb = cb_check_courses;
		cb->setChecked(true);
		tb->addWidget(cb);
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
		connect(cb, &QCheckBox::stateChanged, this, update_class_check);
#else
		connect(cb, &QCheckBox::checkStateChanged, this, update_class_check);
#endif
	}
	update_class_check();

	//auto *menu = dlg->menuBar();
	//auto *a_draw = menu->actionForPath("draw");
	//a_draw->setText(tr("&Draw"));
	//a_draw->addActionInto(ui->actSave);
}

void DrawingGanttWidget::load(int stage_id)
{
	setTitle(tr("E%1 Draw tool").arg(stage_id));
	m_ganttScene->load(stage_id);
}

void drawing::DrawingGanttWidget::on_actSave_triggered()
{
	if(QMessageBox::information(this, tr("Save classes start times"),
								tr("All the user edited classes start times will be overridden.\n"
								   "Do you want to save your changes?"),
								QMessageBox::Save | QMessageBox::Cancel,
								QMessageBox::Save))
	{

		m_ganttScene->save();
	}
}

void DrawingGanttWidget::on_actFind_triggered()
{
	QString txt = m_edFind->text().trimmed().toUpper();
	for(QGraphicsItem *it : m_ganttScene->items()) {
		if(auto *cit = dynamic_cast<ClassItem *>(it)) {
			if(cit->data().className().toUpper().contains(txt)) {
				m_ganttScene->clearSelection();
				cit->setSelected(true);
				return;
			}
		}
	}
}

