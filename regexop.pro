#    libregexop
#    Copyright (C) 2013, 2014  Miguel Barrio Orsikowsky
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

QT -= gui

TARGET = regexop

TEMPLATE = lib

DEFINES += REGEXOP_LIBRARY

SOURCES += regexop.cpp \
    substitution.cpp

HEADERS += regexop.h \
    regexop_global.h \
    substitution.h

INCLUDEPATH += /opt/IBM/InformationServer/Server/PXEngine/include

QMAKE_CXXFLAGS += -Wno-deprecated \
    -Wno-unused-parameter
