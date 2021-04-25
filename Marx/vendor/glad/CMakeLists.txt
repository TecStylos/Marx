cmake_minimum_required (VERSION 3.8)

project ("GLAD")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(
	GLAD_LIB_FILES
	"src/glad.c"
)

set(
	GLAD_INCLUDE_DIRECTORIES
	"include"
)

add_library(
	GLAD STATIC 
	${GLAD_LIB_FILES}
)

target_include_directories(
	GLAD PUBLIC 
	${GLAD_INCLUDE_DIRECTORIES}
)