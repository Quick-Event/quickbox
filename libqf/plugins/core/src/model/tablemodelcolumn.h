#ifndef TABLEMODELCOLUMN_H
#define TABLEMODELCOLUMN_H

#include <qf/core/model/tablemodel.h>

#include <QObject>

namespace qf {
namespace core {
namespace qml {

class TableModelColumn : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString fieldName READ fieldName WRITE setFieldName FINAL)
	Q_PROPERTY(QString caption READ caption WRITE setCaption FINAL)
	Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly FINAL)
	Q_PROPERTY(int castType READ castType WRITE setCastType FINAL)
public:
	explicit TableModelColumn(QObject *parent = 0);
	~TableModelColumn() Q_DECL_OVERRIDE;

	const qf::core::model::TableModel::ColumnDefinition& columnDefinition() {return m_columnDefinition; }

	QString fieldName() const { return m_columnDefinition.fieldName(); }
	void setFieldName(QString arg);

	QString caption() const { return m_columnDefinition.caption(); }
	void setCaption(QString arg);

	bool isReadOnly() const { return m_columnDefinition.isReadOnly(); }
	void setReadOnly(bool arg);

	int castType() const { return m_columnDefinition.castType(); }
	void setCastType(int arg);

	void setColumnIndex(int ix) {m_columnIndex = ix;}
signals:
	void updated(int column_index);
private:
	qf::core::model::TableModel::ColumnDefinition m_columnDefinition;
	int m_columnIndex = -1;
};

}}}

#endif // TABLEMODELCOLUMN_H
