message(including $$PWD)

HEADERS += \
    $$PWD/receiptspartwidget.h \
    $$PWD/receiptswidget.h \
    $$PWD/Receipts/receiptsplugin.h \
    $$PWD/receiptsprinteroptions.h \
    $$PWD/receiptsprinteroptionsdialog.h \
    $$PWD/receiptsprinter.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/receiptspartwidget.cpp \
    $$PWD/receiptswidget.cpp \
    $$PWD/Receipts/receiptsplugin.cpp \
    $$PWD/receiptsprinteroptions.cpp \
    $$PWD/receiptsprinteroptionsdialog.cpp \
    $$PWD/receiptsprinter.cpp

FORMS += \
    $$PWD/receiptswidget.ui \
    $$PWD/receiptsprinteroptionsdialog.ui
