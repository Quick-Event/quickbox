#include "combobox.h"

#include "datacontroller.h"

#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>

#include <QLineEdit>

using namespace qf::qmlwidgets;

//===============================================================
//              ComboBox
//===============================================================
ComboBox::ComboBox(QWidget *parent)
	: Super(parent), IDataWidget(this)
{
	connect(this, &ComboBox::currentTextChanged, this, &ComboBox::onCurrentTextChanged);
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentDataChanged_helper(int)));
	connect(this, SIGNAL(activated(int)), this, SLOT(currentDataActivated_helper(int)));
}

ComboBox::~ComboBox()
{

}

void ComboBox::setCurrentData(const QVariant &val)
{
	QVariant old_val = currentData();
	if(old_val != val) {
		for(int i=0; i<count(); i++) {
			QVariant v = itemData(i);
			if(v == val) {
				setCurrentIndex(i);
				emit currentDataChanged(val);
				break;
			}
		}
	}
}

void ComboBox::insertItem(int index, const QString &text, const QVariant &user_data)
{
	Super::insertItem(index, text, user_data);
}

void ComboBox::setItems(const QVariantList &items)
{
	blockSignals(true);
	clear();
	for(auto v : items) {
		QVariantList vl = v.toList();
		if(vl.isEmpty()) {
			Super::addItem(v.toString());
		}
		else {
			Super::addItem(vl.value(0).toString(), vl.value(1));
		}
	}
	setCurrentIndex(-1);
	blockSignals(false);
}

QVariant ComboBox::dataValue()
{
	QVariant ret;
	if(isEditable() && !isValueRestrictedToItems()) {
		ret = lineEdit()->text();
	}
	else {
		ret = currentData();
	}
	return ret;
}

void ComboBox::setDataValue(const QVariant &val)
{
	qfLogFuncFrame();
	bool emit_change = false;
	int old_ix = currentIndex();
	QString old_text;
	if(isEditable()) {
		old_text = lineEdit()->text();
	}
	loadItems();
	int ix = findData(val);
	setCurrentIndex(ix);
	emit_change = ix != old_ix;
	if(ix < 0 && isEditable() && !isValueRestrictedToItems()) {
		lineEdit()->setText(val.toString());
		emit_change = emit_change || (lineEdit()->text() != old_text);
	}
	if(emit_change) {
		saveDataValue();
		emit dataValueChanged(val);
	}
}

void ComboBox::removeItems()
{
	Super::clear();
}

void ComboBox::loadItems(bool force)
{
	Q_UNUSED(force)
}

void ComboBox::onCurrentTextChanged(const QString &txt)
{
	if(!m_loadingState) {
		qfLogFuncFrame() << txt;
		saveDataValue();
		emit dataValueChanged(dataValue());
	}
}

void ComboBox::currentDataChanged_helper(int ix)
{
	emit currentDataChanged(itemData(ix));
}

void ComboBox::currentDataActivated_helper(int ix)
{
	emit currentDataActivated(itemData(ix));
}

//===============================================================
//              ForeignKeyComboBox
//===============================================================
ForeignKeyComboBox::ForeignKeyComboBox(QWidget *parent)
	: Super(parent)
{
}

ForeignKeyComboBox::~ForeignKeyComboBox()
{

}

void ForeignKeyComboBox::removeItems()
{
	Super::removeItems();
	m_itemsLoaded = false;
}

void ForeignKeyComboBox::loadItems(bool force)
{
	static QString referencedTablePlaceHolder = QStringLiteral("{{referencedTable}}");
	static QString referencedFieldPlaceHolder = QStringLiteral("{{referencedField}}");
	static QString referencedCaptionFieldPlaceHolder = QStringLiteral("{{captionField}}");
	if(force)
		removeItems();
	if(!m_itemsLoaded) do {
		QString connection_name;
		qfLogFuncFrame() << this << "data controler:" << m_dataController;
		if(!m_dataController) {
			connection_name = QLatin1String(QSqlDatabase::defaultConnection);
			qfDebug() << "Data controller is NULL, using default connection name:" << connection_name;
		}
		m_loadingState = true;
		{
			QString tblname = referencedTable();
			QString fldname = referencedField();
			QString capfldname = referencedCaptionField();
			if(capfldname.isEmpty())
				capfldname = fldname;
			QString query_str = queryString();
			if(query_str.isEmpty()) {
				qf::core::sql::QueryBuilder qb;
				qb.select2(tblname, fldname);
				qb.from(tblname);
				if(capfldname != fldname) {
					qb.select(capfldname);
					qb.orderBy(capfldname);
				}
				else {
					qb.orderBy(fldname);
				}
				query_str = qb.toString();
			}
			else {
				query_str = query_str.replace(referencedTablePlaceHolder, tblname);
				query_str = query_str.replace(referencedFieldPlaceHolder, fldname);
				query_str = query_str.replace(referencedCaptionFieldPlaceHolder, capfldname);
			}
			query_str = query_str.trimmed();
			if(!query_str.isEmpty()) {
				qf::core::sql::Query q(connection_name);
				qfDebug() << "\t query_str:" << query_str;
				QSet<QString> field_captions;
				QString caption_format = itemCaptionFormat();
				if(!caption_format.isEmpty()) {
					qfDebug() << "\t itemCaptionFormat:" << caption_format;
					if(caption_format.contains(referencedTablePlaceHolder, Qt::CaseInsensitive))
						caption_format.replace(referencedTablePlaceHolder, tblname, Qt::CaseInsensitive);
					if(caption_format.contains(referencedFieldPlaceHolder, Qt::CaseInsensitive))
						caption_format.replace(referencedFieldPlaceHolder, "{{" + fldname + "}}", Qt::CaseInsensitive);
					if(caption_format.contains(referencedCaptionFieldPlaceHolder, Qt::CaseInsensitive))
						caption_format.replace(referencedCaptionFieldPlaceHolder, "{{" + capfldname + "}}", Qt::CaseInsensitive);
					field_captions = qf::core::Utils::findCaptions(caption_format);
					/*
					QRegExp rx;
					rx.setPattern("\\{\\{([A-Za-z][A-Za-z0-9]*(\\.[A-Za-z][A-Za-z0-9]*)*)\\}\\}");
					rx.setPatternSyntax(QRegExp::RegExp);
					int ix = 0;
					while((ix = rx.indexIn(caption_format, ix)) != -1) {
						qfDebug() << "\t caption:" << rx.cap(0) << rx.cap(1);
						caption_fields << rx.cap(1);
						ix += rx.matchedLength();
					}
					*/
				}
				qfDebug() << "\t capname:" << capfldname;
				q.exec(query_str);
				while(q.next()) {
					QString caption = caption_format;
					Q_FOREACH(QString fld, field_captions) {
						qfDebug() << "\t replacing caption field:" << fld;
						QVariant fld_val = q.value(fld);
						caption = qf::core::Utils::replaceCaptions(caption, fld, fld_val);
					}
					QVariant id = q.value(fldname);
					/*
					QVariant decor;
					if(!decorationField().isEmpty()) {
						decor = q.value(decorationField());
						QString decor_str = decor.toString();
						if(decor_str.startsWith("#")) {
							QColor c;
							c.setNamedColor(decor_str);
							decor = c;
						}
					}
					*/
					addItem(caption, id);
					//cached_items << CachedItem(caption, id, decor);
					//qfTrash() << "\t adding item:" << caption << id.toString() << decor.toString();
				}
			}
		}
		m_itemsLoaded = true;
		qfDebug() << "\t item count:" << count();
		m_loadingState = false;
	} while(false);
}

