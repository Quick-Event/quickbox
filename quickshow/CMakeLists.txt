add_executable(quickshow
    src/appclioptions.cpp
    src/application.cpp
    src/cellrenderer.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/model.cpp
    src/table.cpp
    quickshow.rc
    )

if(USE_QT6)
    qt6_add_translations(quickshow TS_FILES
        quickshow-cs_CZ.ts
        quickshow-fr_FR.ts
        quickshow-nb_NO.ts
        quickshow-nl_BE.ts
        quickshow-pl_PL.ts
        quickshow-ru_RU.ts
        quickshow-uk_UA.ts
        )
else()
    qt5_add_translation(QM_FILES
        quickshow-cs_CZ.ts
        quickshow-fr_FR.ts
        quickshow-nb_NO.ts
        quickshow-nl_BE.ts
        quickshow-pl_PL.ts
        quickshow-ru_RU.ts
        quickshow-uk_UA.ts
        )
    target_sources(quickshow PRIVATE ${QM_FILES})
endif()

target_link_libraries(quickshow PRIVATE Qt::Widgets Qt::Sql libqfcore)
target_include_directories(quickshow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS quickshow)
