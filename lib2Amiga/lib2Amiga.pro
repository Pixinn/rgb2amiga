#-------------------------------------------------
#
# Project created by QtCreator 2014-11-10T00:12:52
#
#-------------------------------------------------

QT       -= gui

TARGET = 2Amiga
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += /usr/include/ImageMagick/ \
               $$PWD/include/

LIBS += -lMagick++ -lMagickCore -lMagickWand

SOURCES +=  src/C2Amiga.cpp \
            src/CPalette.cpp

HEADERS += include/C2Amiga.h \
           src/CPalette.h


### Output ###
system( mkdir /tmp/.obj; mkdir /tmp/.moc )
    DESTDIR =        $$PWD/lib/
    OBJECTS_DIR =    /tmp/.obj
    MOC_DIR =        /tmp/.moc


unix {
    target.path = $$PWD/lib/
    INSTALLS += target
}
