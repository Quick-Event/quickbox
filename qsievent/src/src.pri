INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../libqf/libqfqmlwidgets/include
 message(INCLUDEPATH: $$INCLUDEPATH)

SOURCES += \
	$$PWD/main.cpp\
	$$PWD/mainwindow.cpp \
    $$PWD/application.cpp

HEADERS  += \
	$$PWD/mainwindow.h \
    $$PWD/application.h

FORMS +=

OTHER_FILES += \
	$$PWD/main.qml
