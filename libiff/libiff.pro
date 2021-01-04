#-------------------------------------------------
#
# Project created by QtCreator 2014-11-09T00:11:21
#
#-------------------------------------------------

QT       -= gui

TARGET = libiff
TEMPLATE = lib
CONFIG += staticlib

#INCLUDEPATH += /usr/include/i386-linux-gnu

#QMAKE_CXXFLAGS += -m32

SOURCES +=  src/libiff/cat.c \
            src/libiff/chunk.c \
            src/libiff/error.c \
            src/libiff/extension.c \
            src/libiff/form.c \
            src/libiff/group.c \
            src/libiff/id.c \
            src/libiff/iff.c \
            src/libiff/io.c \
            src/libiff/list.c \
            src/libiff/prop.c \
            src/libiff/rawchunk.c \
            src/libiff/util.c

HEADERS +=  src/libiff/cat.h \
            src/libiff/chunk.h \
            src/libiff/error.h \
            src/libiff/extension.h \
            src/libiff/form.h \
            src/libiff/group.h \
            src/libiff/id.h \
            src/libiff/iff.h \
            src/libiff/io.h \
            src/libiff/list.h \
            src/libiff/prop.h \
            src/libiff/rawchunk.h \
            src/libiff/util.h

### Output ###
system( mkdir /tmp/.obj; mkdir /tmp/.moc )
    DESTDIR =        $$PWD/lib/
    OBJECTS_DIR =    /tmp/.obj
    MOC_DIR =        /tmp/.moc


unix {
    target.path = $$PWD/lib
    INSTALLS += target
}
