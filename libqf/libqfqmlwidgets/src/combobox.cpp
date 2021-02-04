#include "combobox.h"

#include "datacontroller.h"

#include <qf/core/sql/dbenumcache.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/log.h>

#include <QLineEdit>
#include <QSet>

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
	{
		//QSignalBlocker(this); do not work, don't know why, using m_loadingState hack instead
		int old_ix = currentIndex();
		QString old_text;
		if(isEditable()) {
			old_text = lineEdit()->text();
		}
		loadItems();
		m_ignoreIndexChangedSignals = true;
		int ix = findData(val);
		setCurrentIndex(ix);
		emit_change = ix != old_ix;
		if(ix < 0 && isEditable() && !isValueRestrictedToItems()) {
			lineEdit()->setText(val.toString());
			emit_change = emit_change || (lineEdit()->text() != old_text);
		}
		m_ignoreIndexChangedSignals = false;
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
	if(!m_ignoreIndexChangedSignals) {
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
		m_ignoreIndexChangedSignals = true;
		{
			QString tblname = referencedTable();
			QString fldname = referencedField();
			QString capfldname = referencedCaptionField();
			if(capfldname.isEmpty())
				capfldname = fldname;
			QString query_str = queryString();
			if(query_str.isEmpty()) {
				if(!tblname.isEmpty() && !fldname.isEmpty()) {
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
		m_ignoreIndexChangedSignals = false;
	} while(false);
}

//===============================================================
//              DbEnumComboBox
//===============================================================
DbEnumComboBox::DbEnumComboBox(QWidget *parent)
	: Super(parent)
{
}

void DbEnumComboBox::removeItems()
{
	Super::removeItems();
	m_itemsLoaded = false;
}

void DbEnumComboBox::loadItems(bool force)
{
	qfLogFuncFrame() << "force:" << force;
	if(force)
		removeItems();
	if(!m_itemsLoaded) {
		QString connection_name;
		qfLogFuncFrame() << this << "data controler:" << m_dataController;
		if(!m_dataController) {
			connection_name = QLatin1String(QSqlDatabase::defaultConnection);
			qfDebug() << "Data controller is NULL, using default connection name:" << connection_name;
		}
		m_ignoreIndexChangedSignals = true;
		{
			qf::core::sql::DbEnumCache& db_enum_cache = qf::core::sql::DbEnumCache::instanceForConnection(connection_name);
			Q_FOREACH(auto dbe, db_enum_cache.dbEnumsForGroup(groupName())) {
				QString cap = dbe.fillInPlaceholders(itemCaptionFormat());
				addItem(cap, dbe.groupId());
				QColor c = dbe.color();
				if(c.isValid()) {
					setItemData(count() - 1, c, Qt::BackgroundRole);
					setItemData(count() - 1, qf::core::model::TableModel::contrastTextColor(c), Qt::TextColorRole);
				}
			}
		}
		m_itemsLoaded = true;
		qfDebug() << "\t item count:" << count();
		m_ignoreIndexChangedSignals = false;
	};
}
