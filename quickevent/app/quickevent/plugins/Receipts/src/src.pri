message(including $$PWD)

HEADERS += \
    $$PWD/receiptswidget.h \
    $$PWD/receiptsplugin.h \
	#$$PWD/receiptsprinteroptions.h \
    $$PWD/receiptsprinteroptionsdialog.h \
    $$PWD/receiptssettings.h \
    $$PWD/receiptssettingspage.h \
    $$PWD/receiptsprinter.h

SOURCES += \
    $$PWD/receiptswidget.cpp \
    $$PWD/receiptsplugin.cpp \
	#$$PWD/receiptsprinteroptions.cpp \
    $$PWD/receiptsprinteroptionsdialog.cpp \
    $$PWD/receiptssettings.cpp \
    $$PWD/receiptssettingspage.cpp \
    $$PWD/receiptsprinter.cpp

FORMS += \
    $$PWD/receiptswidget.ui \
    $$PWD/receiptssettingspage.ui \
    $$PWD/receiptsprinteroptionsdialog.ui
