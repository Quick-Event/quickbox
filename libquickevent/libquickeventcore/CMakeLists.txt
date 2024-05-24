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
    src/runstatus.cpp
    src/si/checkedcard.cpp
    src/si/checkedpunch.cpp
    src/si/punchrecord.cpp
    src/si/readcard.cpp
    src/si/siid.cpp
    src/utils.cpp
	libquickeventcore.qrc
    )

if(USE_QT6)
    qt6_add_translation(QM_FILES
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
endif()

target_sources(libquickeventcore PRIVATE ${QM_FILES})
target_compile_definitions(libquickeventcore PRIVATE QUICKEVENTCORE_BUILD_DLL)
target_link_libraries(libquickeventcore PUBLIC Qt::Core Qt::Sql libqfcore libsiut)
target_include_directories(libquickeventcore PUBLIC include)

install(TARGETS libquickeventcore)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_BINDIR}/translations)
