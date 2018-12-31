#include "drawingganttwidget.h"
#include "ui_drawingganttwidget.h"

#include "classitem.h"
#include "ganttscene.h"

#include <qf/qmlwidgets/style.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <QLineEdit>

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
	m_edFind->setMaximumWidth(QFontMetrics(font()).width('X') * 8);
	connect(m_edFind, &QLineEdit::textEdited, this, &DrawingGanttWidget::on_actFind_triggered);
	tb->addWidget(m_edFind);
	tb->addAction(ui->actFind);

	auto *menu = dlg->menuBar();
	auto *a_file = menu->actionForPath("draw");
	a_file->setText(tr("&Draw"));
	a_file->addActionInto(ui->actSave);
	//a_file->addActionInto(ui->actFind);
}

void DrawingGanttWidget::load(int stage_id)
{
	setTitle(tr("E%1 Draw tool").arg(stage_id));
	m_ganttScene->load(stage_id);
}

void drawing::DrawingGanttWidget::on_actSave_triggered()
{
	m_ganttScene->save();
}

void DrawingGanttWidget::on_actFind_triggered()
{
	QString txt = m_edFind->text().trimmed().toUpper();
	for(QGraphicsItem *it : m_ganttScene->items()) {
		if(ClassItem *cit = dynamic_cast<ClassItem *>(it)) {
			if(cit->data().className().toUpper().contains(txt)) {
				m_ganttScene->clearSelection();
				cit->setSelected(true);
				return;
			}
		}
	}
}
