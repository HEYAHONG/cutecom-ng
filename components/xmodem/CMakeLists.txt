include(${COMPONENT_CMAKE_PATH})

file(GLOB XMODEM_CXX_C_FILES xmodem/*.cpp xmodem/*.h )
add_library(xmodem ${XMODEM_CXX_C_FILES})
target_include_directories(xmodem
PUBLIC xmodem/
)

components_add_library(xmodem)
