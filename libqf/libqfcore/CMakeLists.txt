add_library(libqfcore SHARED
    src/core/collator.cpp
    src/core/exception.cpp
    src/core/log.cpp
    src/core/logentrymap.cpp
    src/core/stacktrace.cpp
    src/core/string.cpp
    src/core/utils.cpp
    src/model/datadocument.cpp
    src/model/logtablemodel.cpp
    src/model/sqldatadocument.cpp
    src/model/sqltablemodel.cpp
    src/model/tablemodel.cpp
    src/network/networkaccessmanager.cpp
    src/network/networkreply.cpp
    src/sql/catalog.cpp
    src/sql/connection.cpp
    src/sql/dbenum.cpp
    src/sql/dbenumcache.cpp
    src/sql/dbfsattrs.cpp
    src/sql/dbfsdriver.cpp
    src/sql/query.cpp
    src/sql/querybuilder.cpp
    src/sql/tablelocker.cpp
    src/sql/transaction.cpp
    src/utils/clioptions.cpp
    src/utils/crypt.cpp
    src/utils/csvreader.cpp
    src/utils/fileutils.cpp
    src/utils/htmlutils.cpp
    src/utils/settings.cpp
    src/utils/table.cpp
    src/utils/timescope.cpp
    src/utils/treeitembase.cpp
    src/utils/treetable.cpp
	images/qfcore_images.qrc
    )

target_link_libraries(libqfcore PUBLIC libnecrolog Qt::Core Qt::Sql PRIVATE Qt::Gui Qt::Qml Qt::Xml libnecrolog)
target_include_directories(libqfcore PUBLIC include)
target_compile_definitions(libqfcore PRIVATE QFCORE_BUILD_DLL)

if(WIN32)
    # Need these for some WinAPI calls
    target_compile_definitions(libqfcore PRIVATE UNICODE)
    target_compile_definitions(libqfcore PRIVATE _UNICODE)
endif()

install(TARGETS libqfcore)
