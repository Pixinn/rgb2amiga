#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T00:11:21
#
#-------------------------------------------------

QT       -= gui

TARGET = libilbm
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../libiff/src


SOURCES +=  src/libilbm/bitmapheader.c \
            src/libilbm/byterun.c \
            src/libilbm/colormap.c \
            src/libilbm/colorrange.c \
            src/libilbm/cycleinfo.c \
            src/libilbm/destmerge.c \
            src/libilbm/drange.c \
            src/libilbm/grab.c \
            src/libilbm/ilbm.c \
            src/libilbm/ilbmimage.c \
            src/libilbm/interleave.c \
            src/libilbm/sprite.c \
            src/libilbm/viewport.c

HEADERS +=  src/libilbm/bitmapheader.h \
            src/libilbm/byterun.h \
            src/libilbm/colormap.h \
            src/libilbm/colorrange.h \
            src/libilbm/cycleinfo.h \
            src/libilbm/destmerge.h \
            src/libilbm/drange.h \
            src/libilbm/grab.h \
            src/libilbm/ilbm.h \
            src/libilbm/ilbmimage.h \
            src/libilbm/interleave.h \
            src/libilbm/sprite.h \
            src/libilbm/viewport.h

### Output ###
system( mkdir /tmp/.obj; mkdir /tmp/.moc )
    DESTDIR =        $$PWD/lib/
    OBJECTS_DIR =    /tmp/.obj
    MOC_DIR =        /tmp/.moc


unix {
    target.path = $$PWD/lib
    INSTALLS += target
}
