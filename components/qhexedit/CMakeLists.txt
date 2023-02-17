include(${COMPONENT_CMAKE_PATH})

file(GLOB QHEXEDIT2_CXX_C_FILES qhexedit2/src/*.cpp qhexedit2/src/*.h )
add_library(qhexedit2 ${QHEXEDIT2_CXX_C_FILES})
target_include_directories(qhexedit2
PUBLIC qhexedit2/src
)

option(USE_QT6 "Use Qt6" OFF)
option(USE_QT5 "Use Qt5(if USE_QT6 is set,this option will be invalid)" OFF)
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets)
if(USE_QT6)
        #依赖Qt6库
        if(NOT Qt6_FOUND)
        message(FATAL_ERROR "Qt6 is not found")
        endif()
        set(USE_QT_VERSION 6)
        target_link_libraries(qhexedit2 Qt6::Core Qt6::Gui Qt6::Widgets)
elseif(USE_QT5)
        #依赖Qt5库
        if(NOT Qt5_FOUND)
        message(FATAL_ERROR "Qt5 is not found")
        endif()
        set(USE_QT_VERSION 5)
        target_link_libraries(qhexedit2 Qt5::Core Qt5::Gui Qt5::Widgets)
else()
        if(Qt5_FOUND)
        set(USE_QT_VERSION 5)
        target_link_libraries(qhexedit2 Qt5::Core Qt5::Gui Qt5::Widgets)
        elseif(Qt6_FOUND)
        set(USE_QT_VERSION 6)
        target_link_libraries(qhexedit2 Qt6::Core Qt6::Gui Qt6::Widgets)
        else()
        message(FATAL_ERROR "Qt is not found")
        endif()
endif()


components_add_library(qhexedit2)
