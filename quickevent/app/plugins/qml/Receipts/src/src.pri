message(including $$PWD)

HEADERS += \
    $$PWD/receiptspartwidget.h \
    $$PWD/receiptswidget.h \
    $$PWD/Receipts/receiptsplugin.h \
    $$PWD/receiptsprinteroptions.h \
    $$PWD/receiptsprinteroptionsdialog.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/receiptspartwidget.cpp \
    $$PWD/receiptswidget.cpp \
    $$PWD/Receipts/receiptsplugin.cpp \
    $$PWD/receiptsprinteroptions.cpp \
    $$PWD/receiptsprinteroptionsdialog.cpp

FORMS += \
    $$PWD/receiptswidget.ui \
    $$PWD/receiptsprinteroptionsdialog.ui
