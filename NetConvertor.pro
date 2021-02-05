QT -= gui
QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

EXTRA_CXX_FLAGS = -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -std=c++11 #-Wsign-promo
EXTRA_C_FLAGS = -std=gnu11 # -std=c11

EXTRA_SHARE_FLAGS = -Wall -Wextra -pedantic -Wtype-limits -Wempty-body -Wmaybe-uninitialized -Warray-bounds \
            -Wcast-align  -Wcast-qual  -Wenum-compare -Wfloat-equal \
            -Wredundant-decls #-Wsign-conversion  -Wconversion

QMAKE_CXXFLAGS += $${EXTRA_SHARE_FLAGS} $${EXTRA_CXX_FLAGS}
QMAKE_CFLAGS += $${EXTRA_SHARE_FLAGS} $${EXTRA_C_FLAGS}

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS += -std=c11

lessThan(QT_MAJOR_VERSION, 5):error("This project reguire QT >= 5.0")

OBJECTS_DIR = temp/obj
MOC_DIR = temp/moc
INCLUDEPATH +=src/



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/httpListener.cpp \
        src/main.cpp \
        src/Settings.cpp \
        src/netconvertor.cpp \
        src/shared_code.c \
        src/udpListener.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/httpListener.h \
    src/Settings.h \
    src/netconvertor.h \
    src/shared_code.h \
    src/udpListener.h

DISTFILES += \
    ../debug/settings.ini
    ../release/settings.ini
