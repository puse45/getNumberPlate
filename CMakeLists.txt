cmake_minimum_required(VERSION 3.6)
project(getNumberPlate)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Debug)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

#set(OpenCV_ROOT C:/opencv/build)
#set(OpenCV_INCLUDE_DIRS ${OpenCV_ROOT}/include)
#set(OpenCV_LIBRARIES "${OpenCV_ROOT}/lib")
#message("find ${OpenCV_INCLUDE_DIRS}")
#find_package(alpr REQUIRED)
find_package(OpenCV REQUIRED)

set (CMAKE_PREFIX_PATH "C:\\Qt\\5.11.1\\msvc2017_64\\")

#set(CMAKE_PREFIX_PATH "C:/Qt5/5.11.1/msvc2017_64/lib/cmake")
#find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent REQUIRED)
set(ALPR_LIBRARY  "C:/openalpr/openalpr.lib")
set(ALPR_INCLUDE_PATH "C:/openalpr/include/")
set(CMAKE_CXX_STANDARD 11)
MESSAGE(STATUS "Path to header file: " ${ALPR_INCLUDE_PATH})
MESSAGE(STATUS "Path to alpr libraries: " ${ALPR_LIBRARY})
INCLUDE_DIRECTORIES(${ALPR_INCLUDE_PATH})
#target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
#add_executable(${PROJECT_NAME} readfolder.cpp)
add_executable(${PROJECT_NAME} main.cpp detectVehicle.cpp player.cpp player.h)
#add_executable(${PROJECT_NAME} r)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${ALPR_LIBRARY})
#target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)


#target_link_libraries(${PROJECT_NAME} openalpr)


