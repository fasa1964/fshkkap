#-------------------------------------------------
#
# Project created by QtCreator 2020-08-19T16:32:29
#
#-------------------------------------------------

QT       += core gui network
QT       += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FSHKAP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        fshkwindow.cpp \
    appwidget.cpp \
    classbetrieb.cpp \
    classfrage.cpp \
    classlehrling.cpp \
    classprojekt.cpp \
    classskills.cpp \
    formcompany.cpp \
    formapprentice.cpp \
    formcompanylist.cpp \
    formproject.cpp \
    formskills.cpp \
    formallocate.cpp \
    formevaluation.cpp \
    dialogapprenticelist.cpp \
    formquestionidentsettings.cpp \
    classprinting.cpp \
    formapprenticeresultlist.cpp \
    dialoginfo.cpp

HEADERS += \
        fshkwindow.h \
    appwidget.h \
    classbetrieb.h \
    classfrage.h \
    classlehrling.h \
    classprojekt.h \
    classskills.h \
    formcompany.h \
    formapprentice.h \
    formcompanylist.h \
    formproject.h \
    formskills.h \
    formallocate.h \
    formevaluation.h \
    dialogapprenticelist.h \
    formquestionidentsettings.h \
    classprinting.h \
    formapprenticeresultlist.h \
    dialoginfo.h

FORMS += \
        fshkwindow.ui \
    appwidget.ui \
    formcompany.ui \
    formapprentice.ui \
    formcompanylist.ui \
    formproject.ui \
    formskills.ui \
    formallocate.ui \
    formevaluation.ui \
    dialogapprenticelist.ui \
    formquestionidentsettings.ui \
    formapprenticeresultlist.ui \
    dialoginfo.ui

RESOURCES += \
    images.qrc

DISTFILES += \
    images/Badge.svg \
    images/Document.svg \
    images/FSHKIcon.svg \
    images/House.svg \
    images/SplashPixmap.svg \
    images/Wrench.svg
