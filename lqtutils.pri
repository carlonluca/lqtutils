#
# MIT License
#
# Copyright (c) 2020-2021 Luca Carlon
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

android {
QT += androidextras
}
SOURCES += \
    $$PWD/lqtutils_ui.cpp \
    $$PWD/lqtutils_freq.cpp \
    $$PWD/lqtutils_fa.cpp
HEADERS += \
    $$PWD/lqtutils_ui.h \
    $$PWD/lqtutils_freq.h \
    $$PWD/lqtutils_fa.h
ios {
SOURCES += $$PWD/lqtutils_ui.mm
}
contains(QT, statemachine) {
SOURCES += $$PWD/lqtutils_fsm.cpp
HEADERS += $$PWD/lqtutils_fsm.h
}
OTHER_FILES += $$PWD/lqtutils_*.h
