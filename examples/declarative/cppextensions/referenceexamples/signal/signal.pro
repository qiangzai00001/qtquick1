QT += quick1

SOURCES += main.cpp \
           person.cpp \
           birthdayparty.cpp 
HEADERS += person.h \
           birthdayparty.h
RESOURCES += signal.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/qtquick1/cppextensions/referenceexamples/signal
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS signal.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtquick1/cppextensions/referenceexamples/signal
INSTALLS += target sources
