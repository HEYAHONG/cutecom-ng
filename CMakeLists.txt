cmake_minimum_required(VERSION 3.8)

Set(PROJECT_NAME cutecom-ng)
Project(${PROJECT_NAME} C CXX ASM)

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
		add_compiler_options(" -utf-8 ")
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

#添加QVTerminal的源代码
file(GLOB QVTERMINAL_CXX_C_FILES libs/QVTerminal/src/*.cpp libs/QVTerminal/src/*.h  libs/QVTerminal/src/vt/*.cpp libs/QVTerminal/src/vt/*.h )
list(APPEND  MAIN_SRCS ${QVTERMINAL_CXX_C_FILES} )
include_directories(
libs/QVTerminal/src
libs/QVTerminal/src/vt
)

#添加可执行文件
add_executable(${PROJECT_NAME} ${MAIN_SRCS})

option(USE_QT6 "Use Qt6" OFF)
if(USE_QT6)
#依赖Qt6库
find_package(Qt6 COMPONENTS Core Gui Widgets SerialPort Qml Quick QuickWidgets)
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets Qt6::SerialPort Qt6::Qml Qt6::Quick Qt6::QuickWidgets)
else()
#依赖Qt5库
find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort Qml Quick QuickWidgets)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::SerialPort Qt5::Qml Qt5::Quick Qt5::QuickWidgets)
endif()

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
set(CPACK_PACKAGE_VERSION "0.5.0.0")
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

