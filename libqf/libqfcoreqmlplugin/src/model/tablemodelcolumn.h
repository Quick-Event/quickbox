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
public:
	explicit TableModelColumn(QObject *parent = 0);
	~TableModelColumn() Q_DECL_OVERRIDE;

	const qf::core::model::TableModel::ColumnDefinition& columnDefinition() {return m_columnDefinition; }

	QString fieldName() const { return m_columnDefinition.fieldName(); }
	void setFieldName(QString arg) { m_columnDefinition.setFieldName(arg); }

	QString caption() const { return m_columnDefinition.caption(); }
	void setCaption(QString arg) { m_columnDefinition.setCaption(arg); }

	bool isReadOnly() const { return m_columnDefinition.isReadOnly(); }
	void setReadOnly(bool arg) { m_columnDefinition.setReadOnly(arg); }

private:
	qf::core::model::TableModel::ColumnDefinition m_columnDefinition;
};

}}}

#endif // TABLEMODELCOLUMN_H
