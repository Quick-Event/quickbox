#ifndef RUNS_FINDRUNNEREDIT_H
#define RUNS_FINDRUNNEREDIT_H

#include <QLineEdit>

class QCompleter;
class FindRunnersModel;

namespace qf { namespace core { namespace utils { class Table; }}}

class FindRunnerEdit : public QLineEdit
{
	Q_OBJECT
private:
	typedef QLineEdit Super;
public:
	FindRunnerEdit(QWidget *parent = nullptr);

	//const qf::core::utils::Table& table() const {return m_table;}
	void setTable(const qf::core::utils::Table &t);

	Q_SIGNAL void runnerSelected(const QVariantMap &runner_values);
private:
	Q_SLOT void onCompleterActivated(const QModelIndex &index);
private:
	FindRunnersModel *m_findRunnersModel = nullptr;
	QCompleter *m_completer = nullptr;
};

#endif
