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
        features/cedd.cpp \
        features/cl.cpp \
        features/dhash.cpp \
        features/extractor.cpp \
        features/fcth.cpp \
        features/jcd.cpp \
        features/orbslam3.cpp \
        features/phash.cpp \
        features/util/ceddfuzzy10.cpp \
        features/util/ceddfuzzy24.cpp \
        features/util/ceddquant.cpp \
        features/util/fcthquant.cpp \
        features/util/fuzzyfcth.cpp \
        main.cpp \
        util/BytesView.cpp \
        util/IscFileHelper.cpp \
        util/containerutil.cpp \
        videoextractor.cpp \
        videoextractorffmpeg.cpp

HEADERS += \
    features/cedd.h \
    features/cl.h \
    features/dhash.h \
    features/extractor.h \
    features/fcth.h \
    features/jcd.h \
    features/orbslam3.h \
    features/phash.h \
    features/util/ceddfuzzy10.h \
    features/util/ceddfuzzy24.h \
    features/util/ceddquant.h \
    features/util/fcthquant.h \
    features/util/fuzzyfcth.h \
    util/BytesView.hpp \
    util/IscFileHelper.hpp \
    util/containerutil.h \
    videoextractor.h \
    videoextractorffmpeg.h
