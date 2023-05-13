add_subdirectory(libqfcore)
add_subdirectory(libqfqmlwidgets)

if(USE_QT6 AND QF_BUILD_QML_PLUGINS)
	add_subdirectory(plugins)
endif()
