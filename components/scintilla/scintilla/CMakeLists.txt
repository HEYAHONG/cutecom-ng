#启用Qt的文件类型（qrc与ui）处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SCINTILLA_CXX_C_FILES
#scintilla
scintilla/qt/ScintillaEdit/ScintillaEdit.cpp
scintilla/qt/ScintillaEdit/ScintillaDocument.cpp
scintilla/qt/ScintillaEdit/../ScintillaEditBase/PlatQt.cpp
scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaQt.cpp
scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaEditBase.cpp
scintilla/qt/ScintillaEdit/../../src/XPM.cxx
scintilla/qt/ScintillaEdit/../../src/ViewStyle.cxx
scintilla/qt/ScintillaEdit/../../src/UniqueString.cxx
scintilla/qt/ScintillaEdit/../../src/UniConversion.cxx
scintilla/qt/ScintillaEdit/../../src/Style.cxx
scintilla/qt/ScintillaEdit/../../src/Selection.cxx
scintilla/qt/ScintillaEdit/../../src/ScintillaBase.cxx
scintilla/qt/ScintillaEdit/../../src/RunStyles.cxx
scintilla/qt/ScintillaEdit/../../src/RESearch.cxx
scintilla/qt/ScintillaEdit/../../src/PositionCache.cxx
scintilla/qt/ScintillaEdit/../../src/PerLine.cxx
scintilla/qt/ScintillaEdit/../../src/MarginView.cxx
scintilla/qt/ScintillaEdit/../../src/LineMarker.cxx
scintilla/qt/ScintillaEdit/../../src/KeyMap.cxx
scintilla/qt/ScintillaEdit/../../src/Indicator.cxx
scintilla/qt/ScintillaEdit/../../src/Geometry.cxx
scintilla/qt/ScintillaEdit/../../src/EditView.cxx
scintilla/qt/ScintillaEdit/../../src/Editor.cxx
scintilla/qt/ScintillaEdit/../../src/EditModel.cxx
scintilla/qt/ScintillaEdit/../../src/Document.cxx
scintilla/qt/ScintillaEdit/../../src/Decoration.cxx
scintilla/qt/ScintillaEdit/../../src/DBCS.cxx
scintilla/qt/ScintillaEdit/../../src/ContractionState.cxx
scintilla/qt/ScintillaEdit/../../src/CharClassify.cxx
scintilla/qt/ScintillaEdit/../../src/CharacterType.cxx
scintilla/qt/ScintillaEdit/../../src/CharacterCategoryMap.cxx
scintilla/qt/ScintillaEdit/../../src/ChangeHistory.cxx
scintilla/qt/ScintillaEdit/../../src/CellBuffer.cxx
scintilla/qt/ScintillaEdit/../../src/CaseFolder.cxx
scintilla/qt/ScintillaEdit/../../src/CaseConvert.cxx
scintilla/qt/ScintillaEdit/../../src/CallTip.cxx
scintilla/qt/ScintillaEdit/../../src/AutoComplete.cxx

scintilla/qt/ScintillaEdit/ScintillaEdit.h
scintilla/qt/ScintillaEdit/ScintillaDocument.h
scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaEditBase.h
scintilla/qt/ScintillaEdit/../ScintillaEditBase/ScintillaQt.h

#lexilla
lexilla/src/Lexilla.cxx
lexilla/src/../lexlib/*.cxx
lexilla/src/../lexers/*.cxx

lexilla/src/../include/Lexilla.h
lexilla/src/../lexers/*.h

)

add_definitions(-DSCINTILLA_QT=1 -DEXPORT_IMPORT_API=   -D_CRT_SECURE_NO_DEPRECATE=1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
add_definitions(-DNDEBUG=1)
endif()
add_library(scintilla ${SCINTILLA_CXX_C_FILES})
target_include_directories(scintilla PUBLIC
scintilla/qt/ScintillaEdit/../ScintillaEditBase
scintilla/qt/ScintillaEdit/../../include
scintilla/qt/ScintillaEdit/
scintilla/qt/ScintillaEdit/../../src
lexilla/src/../include
lexilla/src/../lexlib
lexilla/src/../../scintilla/include
lexilla/src/
icons/
)
target_compile_definitions(scintilla
PUBLIC -DSCINTILLA_QT=1 -DEXPORT_IMPORT_API=   -D_CRT_SECURE_NO_DEPRECATE=1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
target_compile_definitions(scintilla
PUBLIC -DNDEBUG=1)
endif()


option(USE_QT6 "Use Qt6" OFF)
option(USE_QT5 "Use Qt5(if USE_QT6 is set,this option will be invalid)" OFF)
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets Core5Compat)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets)
if(USE_QT6)
	#依赖Qt6库
	if(NOT Qt6_FOUND)
	message(FATAL_ERROR "Qt6 is not found")
	endif()
	set(USE_QT_VERSION 6)
	target_link_libraries(scintilla Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Core5Compat)
elseif(USE_QT5)
	#依赖Qt5库
	if(NOT Qt5_FOUND)
	message(FATAL_ERROR "Qt5 is not found")
	endif()
	set(USE_QT_VERSION 5)
	target_link_libraries(scintilla Qt5::Core Qt5::Gui Qt5::Widgets)
else()
	if(Qt5_FOUND)
	set(USE_QT_VERSION 5)
	target_link_libraries(scintilla Qt5::Core Qt5::Gui Qt5::Widgets)
	elseif(Qt6_FOUND)
	set(USE_QT_VERSION 6)
	target_link_libraries(scintilla Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Core5Compat)
	else()
	message(FATAL_ERROR "Qt is not found")
	endif()
endif()

