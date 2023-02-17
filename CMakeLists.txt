cmake_minimum_required(VERSION 3.13)

#程序名称
Set(PROJECT_NAME cutecom-ng)
#主版本
set(PROJECT_MAJOR_VERSION_STR 0)
#次版本号
set(PROJECT_MINOR_VERSION_STR 5)
#修订号
set(PROJECT_REVISION_VERSION_STR 0)
#构建版本（以时间戳天数作为构建版本）
string(TIMESTAMP PROJECT_BUILD_TIME_SECOND "%s" UTC)
math(EXPR  PROJECT_BUILD_VERSION_STR "${PROJECT_BUILD_TIME_SECOND}/60/60/24"  OUTPUT_FORMAT DECIMAL)
#程序版本
set(PROJECT_VERSION_STR "${PROJECT_MAJOR_VERSION_STR}.${PROJECT_MINOR_VERSION_STR}.${PROJECT_REVISION_VERSION_STR}.${PROJECT_BUILD_VERSION_STR}")
message(STATUS "Version:${PROJECT_VERSION_STR}")
configure_file(Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Version.h NEWLINE_STYLE WIN32)
if(WIN32)
configure_file(Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/win32/Version.h NEWLINE_STYLE WIN32)
endif()

Project(${PROJECT_NAME} C CXX ASM)
Project(${PROJECT_NAME}	VERSION "${PROJECT_VERSION_STR}")

#设置C++标准
set(CMAKE_CXX_STANDARD 17)

#启用Qt的文件类型（qrc与ui）处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#添加主要的源代码
file(GLOB MAIN_CXX_C_FILES *.cpp *.h *.hpp *.c *.ui *.qrc libs/*.cpp libs/*.h libs/*.c)
list(APPEND  MAIN_SRCS ${MAIN_CXX_C_FILES})
if(WIN32)
	if(MSVC)
		file(GLOB_RECURSE MAIN_RC_FILES win32/window_resources_msvc.rc)
		add_compile_options(-utf-8 )
	else()
		file(GLOB_RECURSE MAIN_RC_FILES win32/window_resources.rc)
	endif()
	list(APPEND  MAIN_SRCS ${MAIN_RC_FILES})
endif()
include_directories(
./
libs/
)

#添加qhexedit2的源代码
file(GLOB QHEXEDIT2_CXX_C_FILES libs/qhexedit2/src/*.cpp libs/qhexedit2/src/*.h )
list(APPEND  MAIN_SRCS ${QHEXEDIT2_CXX_C_FILES} )
include_directories(
libs/qhexedit2/src
)


#添加可执行文件
if(WIN32)
add_executable(${PROJECT_NAME} WIN32  ${MAIN_SRCS})
else()
add_executable(${PROJECT_NAME} ${MAIN_SRCS})
endif()


option(USE_QT6 "Use Qt6" OFF)
option(USE_QT5 "Use Qt5(if USE_QT6 is set,this option will be invalid)" OFF)
find_package(Qt6 QUIET COMPONENTS Core Gui Xml Widgets SerialPort Qml Quick QuickWidgets)
find_package(Qt5 QUIET COMPONENTS Core Gui Xml Widgets SerialPort Qml Quick QuickWidgets)
if(USE_QT6)
	#依赖Qt6库
	if(NOT Qt6_FOUND)
	message(FATAL_ERROR "Qt6 is not found")
	endif()
	set(USE_QT_VERSION 6)
	target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Xml Qt6::Widgets Qt6::SerialPort Qt6::Qml Qt6::Quick Qt6::QuickWidgets)
elseif(USE_QT5)
	#依赖Qt5库
	if(NOT Qt5_FOUND)
	message(FATAL_ERROR "Qt5 is not found")
	endif()
	set(USE_QT_VERSION 5)
	target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Xml Qt5::Widgets Qt5::SerialPort Qt5::Qml Qt5::Quick Qt5::QuickWidgets)
else()
	if(Qt5_FOUND)
	set(USE_QT_VERSION 5)
	target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Xml Qt5::Widgets Qt5::SerialPort Qt5::Qml Qt5::Quick Qt5::QuickWidgets)
	elseif(Qt6_FOUND)
	set(USE_QT_VERSION 6)
	target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Xml Qt6::Widgets Qt6::SerialPort Qt6::Qml Qt6::Quick Qt6::QuickWidgets)
	else()
	message(FATAL_ERROR "Qt is not found")
	endif()
endif()


#添加组件(components)
include(cmake/components.cmake)
components_init(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

#安装
include(GNUInstallDirs)
INSTALL(TARGETS ${PROJECT_NAME}
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

#软件包相关
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#安装桌面快捷方式
	install(FILES cutecom-ng.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	#安装桌面快捷方式图标
	install(FILES cutecomng.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
	if(EXISTS "/etc/issue")
		file(READ "/etc/issue" ETC_ISSUE)
		if(ETC_ISSUE MATCHES "Debian|Ubuntu")

#打包deb软件包
message(STATUS "this is a debian(ubuntu) system")
set(CPACK_PACKAGE_NAME  "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION_STR}")
set(CPACK_PACKAGE_VENDOR "HEYAHONG")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cutecom-ng modified by HEYAHONG")
set(CPACK_PACKAGE_DESCRIPTION "cutecom-ng modified by HEYAHONG")
set(CPACK_PACKAGE_CONTACT    "2229388563@qq.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER    "HEYAHONG")
if(NOT USE_QT6)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.15.3-1),libqt5serialport5 (>= 5.15.3-1),libqt5quickwidgets5 (>= 5.15.3-1),libqt5quick5 (>= 5.15.3-1),libqt5qml5 (>= 5.15.3-1),qml-module-qtquick-dialogs (>= 5.15.3-1),qml-module-qtquick-controls (>= 5.15.3-1)")
else()
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6 (>= 6.2.4-1),libqt6serialport6 (>= 6.2.4-1),libqt6quickwidgets6 (>= 6.2.4-1),libqt6quick6 (>= 6.2.4-1),libqt6qml6 (>= 6.2.4-1),qml6-module-qtquick-dialogs (>= 6.2.4-1),qml6-module-qtquick-controls (>= 6.2.4-1)")
endif()
set(CPACK_DEBIAN_PACKAGE_SECTION "Qt;System;X11")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")
set(CPACK_GENERATOR "DEB")
include(CPack)

		endif()
	endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")

#打包NSIS安装包
message(STATUS "this is a windows system")
set(CPACK_PACKAGE_NAME  "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION_STR}")
set(CPACK_PACKAGE_VENDOR "HEYAHONG")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "cutecom-ng modified by HEYAHONG")
set(CPACK_PACKAGE_DESCRIPTION "cutecom-ng modified by HEYAHONG")
set(CPACK_PACKAGE_CONTACT    "2229388563@qq.com")
#设置安装程序图标
set(CPACK_PACKAGE_ICON_PATH_STRING "${CMAKE_CURRENT_SOURCE_DIR}/win32/window_resources.ico")
string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON_PATH_STRING})
set(CPACK_NSIS_MUI_ICON ${CPACK_PACKAGE_ICON})
set(CPACK_NSIS_MUI_UNIICON ${CPACK_PACKAGE_ICON})
#安装前卸载
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
#设置在控制面板的名称
set(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_NAME})
#设置需要创建快捷方式的程序
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME};${PROJECT_NAME} )
#设置LICENSE
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/LICENSE.txt)
#设置Welcome
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/WELCOME.txt)
#设置readme
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/README.txt)
set(CPACK_GENERATOR "NSIS")
include(CPack)

#执行安装脚本(用于使用windeployqt添加Qt依赖)
set(DUMMYQMLDIR  ${CMAKE_CURRENT_SOURCE_DIR}/win32/dummyqml/qt${USE_QT_VERSION}/)
configure_file(win32/windeployqt.cmake.in  ${CMAKE_BINARY_DIR}/windeployqt.cmake @ONLY)
install(SCRIPT ${CMAKE_BINARY_DIR}/windeployqt.cmake)


endif()
