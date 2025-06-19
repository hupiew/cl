QT += core gui multimedia

CONFIG += c++20 console
CONFIG -= app_bundle

INCLUDEPATH += $$PWD/flatbuffers/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += CL_USE_FFMPEG=1

LIBS += -lavcodec -lavformat -lavutil -lswscale

CONFIG(release, debug|release) {
  message( "release" )
  LIBS += -lopencv_world490
}
CONFIG(debug, debug|release) {
  message( "debug" )
  LIBS += -lopencv_world490d
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


SOURCES += \
        ceddfuzzy10.cpp \
        ceddfuzzy24.cpp \
        ceddquant.cpp \
        cl.cpp \
        dhash.cpp \
        extractor.cpp \
        fcth.cpp \
        fcthquant.cpp \
        features/cedd.cpp \
        features/orbslam3.cpp \
        fuzzyfcth.cpp \
        jcd.cpp \
        main.cpp \
        phash.cpp \
        util/BytesView.cpp \
        util/IscFileHelper.cpp \
        util/containerutil.cpp \
        videoextractor.cpp \
        videoextractorffmpeg.cpp

HEADERS += \
    ceddfuzzy10.h \
    ceddfuzzy24.h \
    ceddquant.h \
    cl.h \
    dhash.h \
    extractor.h \
    fcth.h \
    fcthquant.h \
    features/cedd.h \
    features/orbslam3.h \
    fuzzyfcth.h \
    jcd.h \
    phash.h \
    util/BytesView.hpp \
    util/IscFileHelper.hpp \
    util/containerutil.h \
    videoextractor.h \
    videoextractorffmpeg.h
