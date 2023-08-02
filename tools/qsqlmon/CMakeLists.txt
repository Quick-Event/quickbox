add_executable(qsqlmon
    src/columnselectorwidget.cpp
    src/dlgaltertable.cpp
    src/dlgcolumndef.cpp
    src/dlgeditconnection.cpp
    src/dlgindexdef.cpp
    src/driver/qfhttpmysql/qfhttpmysql.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/qfobjectitemmodel.cpp
    src/qfsqlsyntaxhighlighter.cpp
    src/qfstatusbar.cpp
    src/servertreedock.cpp
    src/servertreeitem.cpp
    src/servertreemodel.cpp
    src/servertreeview.cpp
    src/sqldock.cpp
    src/sqltextedit.cpp
    src/tableviewwidget.cpp
	src/theapp.cpp
	src/qsqlmon.qrc
    )

if(USE_QT6)
	qt6_add_translation(QM_FILES
        qsqlmon-cs_CZ.ts
        qsqlmon-fr_FR.ts
        qsqlmon-nb_NO.ts
        qsqlmon-nl_BE.ts
        qsqlmon-pl_PL.ts
        qsqlmon-ru_RU.ts
        qsqlmon-uk_UA.ts
        )
else()
    qt5_add_translation(QM_FILES
        qsqlmon-cs_CZ.ts
        qsqlmon-fr_FR.ts
        qsqlmon-nb_NO.ts
        qsqlmon-nl_BE.ts
        qsqlmon-pl_PL.ts
        qsqlmon-ru_RU.ts
        qsqlmon-uk_UA.ts
        )
endif()

target_sources(qsqlmon PRIVATE ${QM_FILES})
target_link_libraries(qsqlmon PRIVATE libqfqmlwidgets)
target_include_directories(qsqlmon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS qsqlmon)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_BINDIR}/translations)
