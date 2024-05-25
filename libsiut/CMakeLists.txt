add_library(libsiut SHARED
    src/device/crc529.c
    src/device/commport.cpp
    src/device/sidevicedriver.cpp
    src/device/sitask.cpp
    src/message/simessagedata.cpp
    src/sicard.cpp
    src/sipunch.cpp
    )

if(USE_QT6)
	qt6_add_translation(QM_FILES
        libsiut-cs_CZ.ts
        libsiut-fr_FR.ts
        libsiut-nb_NO.ts
        libsiut-nl_BE.ts
        libsiut-pl_PL.ts
        libsiut-ru_RU.ts
        libsiut-uk_UA.ts
        )
else()
    qt5_add_translation(QM_FILES
        libsiut-cs_CZ.ts
        libsiut-fr_FR.ts
        libsiut-nb_NO.ts
        libsiut-nl_BE.ts
        libsiut-pl_PL.ts
        libsiut-ru_RU.ts
        libsiut-uk_UA.ts
        )
endif()

target_sources(libsiut PRIVATE ${QM_FILES})
target_compile_definitions(libsiut PRIVATE SIUT_BUILD_DLL)
target_link_libraries(libsiut PUBLIC Qt::Core Qt::SerialPort libqfcore)
target_include_directories(libsiut PUBLIC include)

install(TARGETS libsiut)
install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_BINDIR}/translations)
