add_library(libquickeventgui SHARED
    src/audio/player.cpp
    src/audio/wavfile.cpp
    src/og/itemdelegate.cpp
    src/og/sqltablemodel.cpp
    src/og/timeedit.cpp
    src/partwidget.cpp
    src/reportoptionsdialog.cpp
    src/si/siidedit.cpp
    libquickeventgui.qrc
    )

if(USE_QT6)
    qt6_add_translations(libquickeventgui TS_FILES
        libquickeventgui-cs_CZ.ts
        libquickeventgui-fr_FR.ts
        libquickeventgui-nb_NO.ts
        libquickeventgui-nl_BE.ts
        libquickeventgui-pl_PL.ts
        libquickeventgui-ru_RU.ts
        libquickeventgui-uk_UA.ts
        )
else()
    qt5_add_translation(QM_FILES
        libquickeventgui-cs_CZ.ts
        libquickeventgui-fr_FR.ts
        libquickeventgui-nb_NO.ts
        libquickeventgui-nl_BE.ts
        libquickeventgui-pl_PL.ts
        libquickeventgui-ru_RU.ts
        libquickeventgui-uk_UA.ts
        )
    target_sources(libquickeventgui PRIVATE ${QM_FILES})
endif()

target_compile_definitions(libquickeventgui PRIVATE QUICKEVENTGUI_BUILD_DLL)
target_link_libraries(libquickeventgui PUBLIC Qt::Core Qt::Multimedia libquickeventcore libqfqmlwidgets)

install(TARGETS libquickeventgui)
