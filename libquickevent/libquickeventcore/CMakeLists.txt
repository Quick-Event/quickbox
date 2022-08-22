add_library(libquickeventcore SHARED
    src/codedef.cpp
    src/coursedef.cpp
    src/exporters/fileexporter.cpp
    src/exporters/htmlfileexporter.cpp
    src/exporters/stageresultscsvexporter.cpp
    src/exporters/stageresultshtmlexporter.cpp
    src/exporters/stagestartlisthtmlexporter.cpp
    src/og/sqltablemodel.cpp
    src/og/timems.cpp
    src/si/checkedcard.cpp
    src/si/checkedpunch.cpp
    src/si/punchrecord.cpp
    src/si/readcard.cpp
    src/si/siid.cpp
    src/utils.cpp
    )

if(USE_QT6)
    qt6_add_translations(libquickeventcore TS_FILES
    libquickeventcore-cs_CZ.ts
    libquickeventcore-fr_FR.ts
    libquickeventcore-nb_NO.ts
    libquickeventcore-nl_BE.ts
    libquickeventcore-pl_PL.ts
    libquickeventcore-ru_RU.ts
    libquickeventcore-uk_UA.ts
    )
else()
    qt5_add_translation(QM_FILES
        libquickeventcore-cs_CZ.ts
        libquickeventcore-fr_FR.ts
        libquickeventcore-nb_NO.ts
        libquickeventcore-nl_BE.ts
        libquickeventcore-pl_PL.ts
        libquickeventcore-ru_RU.ts
        libquickeventcore-uk_UA.ts
        )
    target_sources(libquickeventcore PRIVATE ${QM_FILES})
endif()

target_compile_definitions(libquickeventcore PRIVATE QUICKEVENTCORE_BUILD_DLL)
target_link_libraries(libquickeventcore PUBLIC Qt::Core Qt::Sql libsiut)
target_include_directories(libquickeventcore PUBLIC include)

install(TARGETS libquickeventcore)
