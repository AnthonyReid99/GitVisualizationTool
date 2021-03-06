#-------------------------------------------------
#
# Project created by QtCreator 2014-02-23T20:32:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GitVisualizationTool
TEMPLATE = app
CONFIG += C11++
QMAKE_CXXFLAGS += -std=c++11


SOURCES += branch.cpp \
        commitnode.cpp \
        main.cpp\
        mainwindow.cpp \
        gcommitarrow.cpp \
        gcommitnode.cpp \
        ggraphicsscene.cpp \
        gitapi.cpp \
        gitapiresponse.cpp \
        gitobject.cpp \
        gituser.cpp \
        localrepoparser.cpp \
        localrepository.cpp \
        remote.cpp \
        repository.cpp \
        sha1.cpp \
        tag.cpp \
        gitinitdialog.cpp \
        gitadddialog.cpp \
        gitmergedialog.cpp \
        gitcommitdialog.cpp \
        gitbranchdialog.cpp \
        gitrevertdialog.cpp \
        gitpulldialog.cpp \
        gitpushdialog.cpp \
        logger.cpp \
    gbranchlabel.cpp \
    glabelline.cpp \
    gitopenrepodialog.cpp \
    giterrordialog.cpp \
    gitcheckoutdialog.cpp


HEADERS  += branch.h \
        commitnode.h \
        mainwindow.h \
        gcommitarrow.h \
        gcommitnode.h \
        ggraphicsscene.h \
        gitapi.h \
        gitapiresponse.h \
        gitobject.h \
        gituser.h \
        localrepoparser.h \
        localrepository.h \
        remote.h \
        repository.h \
        sha1.h \
        tag.h \
        gitinitdialog.h \
        gitadddialog.h \
        gitmergedialog.h \
        gitcommitdialog.h \
        gitbranchdialog.h \
        gitrevertdialog.h \
        gitpulldialog.h \
        gitpushdialog.h \
        logger.h \ 
    gbranchlabel.h \
    glabelline.h \
    gitopenrepodialog.h \
    giterrordialog.h \
    gitcheckoutdialog.h


FORMS    += mainwindow.ui \
        gitinitdialog.ui \
        gitadddialog.ui \
        gitmergedialog.ui \
        gitcommitdialog.ui \
        gitbranchdialog.ui \
        gitrevertdialog.ui \
        gitpulldialog.ui \
        gitpushdialog.ui \
    gitopenrepodialog.ui \
    giterrordialog.ui \
    gitcheckoutdialog.ui

RESOURCES += resources/images.qrc
