cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)
project(textfilter)

set(Qt5_DIR ./vendor/usr/lib/x86_64-linux-gnu/cmake/Qt5)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

if(WIN32)
  # see https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories(
    ${PROJECT_SOURCE_DIR}/vendor/usr/include/x86_64-linux-gnu/qt5, 
    ${PROJECT_SOURCE_DIR}/vendor/usr/include/x86_64-linux-gnu/qt5/*, 
    ${PROJECT_SOURCE_DIR}/src
) 

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*")

add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(textfilter PRIVATE Qt5::Widgets)
