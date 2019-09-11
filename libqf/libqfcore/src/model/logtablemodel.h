#ifndef QF_CORE_MODEL_LOGTABLEMODEL_H
#define QF_CORE_MODEL_LOGTABLEMODEL_H

#include "../core/coreglobal.h"
#include "../core/logcore.h"
#include "../core/utils.h"

#include <QAbstractTableModel>

namespace qf {
namespace core {
class LogEntryMap;
namespace model {

class QFCORE_DECL_EXPORT  LogTableModel : public QAbstractTableModel
{
	Q_OBJECT
private:
	typedef QAbstractTableModel Super;
public:
	enum Cols {Severity, Category, Message, TimeStamp, File, Line, Function, UserData, Count};
	class QFCORE_DECL_EXPORT Row {
	public:
		explicit Row() {}
		explicit Row(qf::core::Log::Level severity, const QString& domain, const QString& file, int line, const QString& msg, const QDateTime& time_stamp, const QString& function = QString(), const QVariant &user_data = QVariant());

		QVariant value(int col) const;
		void setValue(int col, const QVariant &v);
	private:
		QVector<QVariant> m_data;
	};
public:
	LogTableModel(QObject *parent = nullptr);

	enum class Direction {AppendToTop, AppendToBottom};

	QF_PROPERTY_IMPL2(Direction, d, D, irection, Direction::AppendToBottom)
	QF_PROPERTY_IMPL2(int, m, M, aximumRowCount, 1000)

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

	void clear();
	Row rowAt(int row) const;
	Q_SLOT void addLogEntry(const qf::core::LogEntryMap &le);
	void addLog(qf::core::Log::Level severity, const QString& category, const QString &file, int line, const QString& msg, const QDateTime& time_stamp, const QString &function = QString(), const QVariant &user_data = QVariant());
	void addRow(const Row &row);
	Q_SIGNAL void logEntryInserted(int row_no);
protected:
	virtual QString prettyFileName(const QString &file_name);
protected:
	QList<Row> m_rows;
};

}}}

#endif // QF_CORE_MODEL_LOGTABLEMODEL_H
