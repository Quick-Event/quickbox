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

qt_add_qml_module(qfcoreplugin
    URI "qf.core"
    VERSION "1.0"
	PLUGIN_TARGET qfcoreplugin
#	QML_FILES ${qml_files}
	RESOURCE_PREFIX :/qf/qml/core
#    PAST_MAJOR_VERSIONS 1
#    CLASS_NAME QtWebSocketsDeclarativeModule
#    NO_PLUGIN_OPTIONAL
#    NO_GENERATE_PLUGIN_SOURCE
#    NO_GENERATE_QMLTYPES
#    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
)


target_sources(qfcoreplugin PRIVATE
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

target_include_directories(qfcoreplugin PRIVATE src/sql)

target_link_libraries(qfcoreplugin PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
	libqfcore
)

