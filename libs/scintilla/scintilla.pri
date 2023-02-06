QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
equals(QT_MAJOR_VERSION, 6): QT += core5compat

CONFIG += c++1z

#scintilla

SOURCES += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase/PlatQt.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaQt.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaEditBase.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/XPM.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/ViewStyle.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/UniqueString.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/UniConversion.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Style.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Selection.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/ScintillaBase.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/RunStyles.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/RESearch.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/PositionCache.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/PerLine.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/MarginView.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/LineMarker.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/KeyMap.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Indicator.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Geometry.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/EditView.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Editor.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/EditModel.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Document.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/Decoration.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/DBCS.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/ContractionState.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CharClassify.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CharacterType.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CharacterCategoryMap.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/ChangeHistory.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CellBuffer.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CaseFolder.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CaseConvert.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/CallTip.cxx \
    $$PWD/scintilla/qt/ScintillaEdit/../../src/AutoComplete.cxx

HEADERS  += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaEditBase.h \
    $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaQt.h

OTHER_FILES +=

INCLUDEPATH += $$PWD/scintilla/qt/ScintillaEdit/../ScintillaEditBase $$PWD/scintilla/qt/ScintillaEdit/../../include $$PWD/scintilla/qt/ScintillaEdit/ $$PWD/scintilla/qt/ScintillaEdit/../../src 

#添加lexilla
SOURCES += \
    $$PWD/lexilla/src/Lexilla.cxx \
    $$files($$PWD/lexilla/src/../lexlib/*.cxx, false) \
    $$files($$PWD/lexilla/src/../lexers/*.cxx, false)

HEADERS  += \
    $$PWD/lexilla/src/../include/Lexilla.h \
    $$files($$PWD/lexilla/src/../lexers/*.h, false)

INCLUDEPATH +=  $$PWD/lexilla/src/../include  $$PWD/lexilla/src/../lexlib  $$PWD/lexilla/src/../../scintilla/include  $$PWD/lexilla/src/

DEFINES += SCINTILLA_QT=1  MAKING_LIBRARY=1 _CRT_SECURE_NO_DEPRECATE=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}
