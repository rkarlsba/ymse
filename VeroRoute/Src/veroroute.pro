#-------------------------------------------------
#
# Project created by QtCreator 2013-07-01T09:05:55
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += optimize_full

TARGET = veroroute
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    mainwindow_drawing.cpp \
    mainwindow_events.cpp \
    renderingdialog.cpp \
    hotkeysdialog.cpp \
    infodialog.cpp \
    controldialog.cpp \
    bomdialog.cpp \
    templatesdialog.cpp \
    pindialog.cpp \
    textdialog.cpp \
    wiredialog.cpp \
    compdialog.cpp \
    Board_routing.cpp \
    Board_components.cpp \
    Board_import.cpp \
    Board.cpp \
    Component.cpp \
    CompDefiner.cpp \
    CurveList.cpp \
    FootPrint.cpp \    
    GWriter.cpp


HEADERS  += mainwindow.h \
    controldialog.h \
    compdialog.h \
    renderingdialog.h \
    hotkeysdialog.h \
    infodialog.h \
    bomdialog.h \
    templatesdialog.h \
    pindialog.h \
    textdialog.h \
    wiredialog.h \
    myscrollarea.h \
    Board.h \
    GuiControl.h \
    Common.h \
    CompTypes.h \
    CompDefiner.h \
    Component.h \
    CurveList.h \
    Pin.h \
    TrackElement.h \
    CompElement.h \
    Element.h \
    Grid.h \
    FootPrint.h \
    AdjInfo.h \
    AdjInfoManager.h \
    ColorManager.h \
    CompManager.h \
    GroupManager.h \
    GPainter.h \
    GWriter.h \
    HistoryManager.h \
    MyRGB.h \
    NodeInfo.h \
    NodeInfoManager.h \
    Template.h \
    TemplateManager.h \
    Persist.h \
    Rect.h \
    RectManager.h \
    Shape.h \
    SimplexFont.h \
    StringHelper.h \
    TextRect.h \
    TextManager.h \
    Transform.h \
    Version.h \
    VrtVersion.h


FORMS    += mainwindow.ui \
    controldialog.ui \
    compdialog.ui \
    renderingdialog.ui \
    hotkeysdialog.ui \
    infodialog.ui \
    bomdialog.ui \
    templatesdialog.ui \
    pindialog.ui \
    textdialog.ui \
    wiredialog.ui
    

RESOURCES     = veroroute.qrc

DESTDIR = ..

DISTFILES +=

QMAKE_INSTALL_FILE    = install -m 644
QMAKE_INSTALL_PROGRAM = install -m 755

unix {
    target.path = $${PREFIX}/usr/local/bin/
    target.files = ../veroroute

    desktopentry.path = $${PREFIX}/usr/local/share/applications
    desktopentry.files = ../veroroute.desktop

    pixmapA.path = $${PREFIX}/usr/local/share/pixmaps
    pixmapA.files = ../veroroute.png

    pixmapB.path = $${PREFIX}/usr/local/share/veroroute
    pixmapB.files = ../veroroute.png

    tutorials.path = $${PREFIX}/usr/local/share/veroroute/tutorials
    tutorials.files = ../tutorials/*

    gedasymbols.path = $${PREFIX}/usr/local/share/gEDA/sym
    gedasymbols.files = ../libraries/gEDA/veroroute_*

    gedalib.path = $${PREFIX}/usr/local/share/gEDA/gafrc.d
    gedalib.files = ../libraries/gEDA/veroroute-clib.scm

    INSTALLS += target desktopentry pixmapA pixmapB tutorials gedasymbols gedalib
}

