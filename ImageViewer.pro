#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T14:14:05
#
#-------------------------------------------------

QT += widgets

HEADERS       = imageviewer.h
SOURCES       = imageviewer.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target

wince {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
