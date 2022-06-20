QT += core gui multimedia

CONFIG += c++2a console
CONFIG -= app_bundle

INCLUDEPATH += $$PWD/flatbuffers/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lavcodec -lavformat -lavutil -lswscale

SOURCES += \
        cedd.cpp \
        ceddfuzzy10.cpp \
        ceddfuzzy24.cpp \
        ceddquant.cpp \
        cl.cpp \
        compactceddquant.cpp \
        dhash.cpp \
        extractor.cpp \
        fcth.cpp \
        fcthquant.cpp \
        fuzzyfcth.cpp \
        jcd.cpp \
        main.cpp \
        phash.cpp \
        util/BytesView.cpp \
        util/IscFileHelper.cpp \
        videoextractor.cpp \
        videoextractorffmpeg.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cedd.h \
    ceddfuzzy10.h \
    ceddfuzzy24.h \
    ceddquant.h \
    cl.h \
    compactceddquant.h \
    dhash.h \
    extractor.h \
    fcth.h \
    fcthquant.h \
    fuzzyfcth.h \
    jcd.h \
    phash.h \
    util/BytesView.hpp \
    util/IscFileHelper.hpp \
    videoextractor.h \
    videoextractorffmpeg.h
