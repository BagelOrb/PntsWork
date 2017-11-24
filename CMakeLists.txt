cmake_minimum_required(VERSION 2.8.3)
project(PntWorks)


SET(GCC_COVERAGE_LINK_FLAGS "-lGL -lGLU -lGLEW -lglut")


find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(GLEW REQUIRED)


include_directories(${GLEW_INCLUDE_PATH})


###########################
# Find Eigen
###############

find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})

###########################
# Find real sense
###############

find_package(realsense2  REQUIRED)

set(DEPENDENCIES realsense)
    if(NOT WIN32)
        list(APPEND DEPENDENCIES m ${LIBUSB1_LIBRARIES})
    endif()


file(GLOB SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLK.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLKGeometry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLKGraph.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLKHeap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLKMatrixLib.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GLKLib/GLKObList.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PntsSetBody.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PntsSetOperation.cpp)

add_executable(${PROJECT_NAME}_bin main.cpp)

target_link_libraries(${PROJECT_NAME}_bin ${SOURCE_FILES} ${GCC_COVERAGE_LINK_FLAGS} ${DEPENDENCIES} )
