cmake_minimum_required (VERSION 3.8)

project ("global")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory("Marx/vendor/imgui")
add_subdirectory("Marx")
add_subdirectory("Sandbox")

if(WIN32)
    add_subdirectory("DonHugo")
endif()