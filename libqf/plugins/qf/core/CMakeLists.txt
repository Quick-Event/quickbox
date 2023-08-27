qt_add_qml_module(qfcoreplugin
    URI "qf.core"
    VERSION "1.0"
	PLUGIN_TARGET qfcoreplugin
	RESOURCE_PREFIX qml/qf/core
#	QML_FILES ${qml_files}
#    PAST_MAJOR_VERSIONS 1
#    NO_PLUGIN_OPTIONAL
	NO_GENERATE_PLUGIN_SOURCE
#    NO_GENERATE_QMLTYPES
#    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
)


target_sources(qfcoreplugin PRIVATE
#	src/qfcore_plugin.h
#	src/qmlfilesingleton.cpp

	src/qfcore_plugin.cpp
	src/crypt.cpp
	src/model/tablemodelcolumn.cpp
	src/model/sqltablemodel.cpp
	src/model/sqldatadocument.cpp
	src/settings.cpp
	src/sql/sqlconnection.cpp
	src/sql/sqlquery.cpp
	src/sql/sqlquerybuilder.cpp
	src/sql/sqlrecord.cpp
	src/sql/qmlsqlsingleton.cpp
	src/qmllogsingleton.cpp
	core.qrc
)

target_include_directories(qfcoreplugin PRIVATE src/sql)

target_link_libraries(qfcoreplugin PUBLIC
    Qt::Core
#    Qt::Gui
    Qt::Qml
	libqfcore
)

set(PLUGIN_DEST_DIR ${CMAKE_INSTALL_BINDIR}/qml/qf/core)
install(TARGETS qfcoreplugin LIBRARY DESTINATION ${PLUGIN_DEST_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir DESTINATION ${PLUGIN_DEST_DIR})


