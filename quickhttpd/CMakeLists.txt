add_executable(quickhttpd
    src/appclioptions.cpp
    src/application.cpp
    src/httpconnection.cpp
    src/httpserver.cpp
    src/main.cpp
    )

target_link_libraries(quickhttpd PRIVATE Qt::Network libqfcore libquickeventcore)

install(TARGETS quickhttpd)
