QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    login_widget.cpp \
    main.cpp \
    menu_widget.cpp \
    pop3_client.cpp \
    sending_mail_widget.cpp \
    smtp_client.cpp

HEADERS += \
    login_widget.h \
    menu_widget.h \
    pop3_client.h \
    sending_mail_widget.h \
    smtp_client.h

FORMS += \
    login_widget.ui \
    menu_widget.ui \
    sending_mail_widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
