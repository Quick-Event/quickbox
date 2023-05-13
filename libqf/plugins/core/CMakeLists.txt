add_library(qfcore SHARED
	src/crypt.cpp
	src/model/tablemodelcolumn.cpp
	src/model/sqltablemodel.cpp
	src/model/sqldatadocument.cpp
	src/qmlfilesingleton.cpp
	src/settings.cpp
	src/sql/sqlconnection.cpp
	src/sql/sqlquery.cpp
	src/sql/sqlquerybuilder.cpp
	src/sql/sqlrecord.cpp
	src/sql/qmlsqlsingleton.cpp
	#src/plugin.cpp
	src/qmllogsingleton.cpp
	core.qrc
)

target_link_libraries(qfcore PUBLIC Qt::Core Qt::Gui Qt::Qml libqfcore)
target_include_directories(qfcore PRIVATE src/sql)

set(qml_files
	qml/Log.qml
	qml/QfObject.qml
	qml/sql/def/Boolean.qml
	qml/sql/def/Date.qml
	qml/sql/def/DateTime.qml
	qml/sql/def/Enum.qml
	qml/sql/def/Field.qml
	qml/sql/def/ForeignKeyReference.qml
	qml/sql/def/Index.qml
	qml/sql/def/Insert.qml
	qml/sql/def/Int.qml
	qml/sql/def/Real.qml
	qml/sql/def/Schema.qml
	qml/sql/def/Serial.qml
	qml/sql/def/String.qml
	qml/sql/def/Table.qml
	qml/sql/def/Time.qml
	qml/sql/def/private/FieldType.qml
	#qml/test.qml
)

qt_add_qml_module(qfcore
    #OUTPUT_DIRECTORY qml/qf/core
    URI qf.core
	VERSION 1.0
    #SOURCES ${cpp_files}
	RESOURCE_PREFIX /
    QML_FILES ${qml_files}
    #RESOURCES ${images}
)
