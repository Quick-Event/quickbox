#ifndef QF_QMLWIDGETS_FRAMEWORK_PARTSWITCH_H
#define QF_QMLWIDGETS_FRAMEWORK_PARTSWITCH_H

#include <QToolBar>
#include <QToolButton>

#include <QPointer>

namespace qf {
namespace qmlwidgets {
namespace framework {

class MainWindow;
class PartWidget;
class StackedCentralWidget;

class PartSwitchToolButton : public QToolButton
{
	Q_OBJECT
	//friend class PartSwitch;
private:
	typedef QToolButton Super;
public:
	PartSwitchToolButton(QWidget *parent = nullptr);
public:
	int partIndex() {return m_partIndex;}
	void setPartIndex(int ix) {m_partIndex = ix;}

	Q_SIGNAL void clicked(int part_index);
private:
	int m_partIndex;
};

class PartSwitch : public QToolBar
{
	Q_OBJECT
private:
	typedef QToolBar Super;
public:
	explicit PartSwitch(StackedCentralWidget *central_widget,QWidget *parent = nullptr);
	~PartSwitch() Q_DECL_OVERRIDE;
public:
	void addPartWidget(PartWidget *widget);

	Q_SIGNAL void partActivated(int ix, bool is_active);
private:
	Q_SLOT void setCurrentPartIndex(int ix, bool is_active = true);
	int buttonCount();
	PartSwitchToolButton* buttonAt(int part_index);
private:
	QPointer<StackedCentralWidget> m_centralWidget;
	int m_currentPartIndex;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_PARTSWITCH_H
