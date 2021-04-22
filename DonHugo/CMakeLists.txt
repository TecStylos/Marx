cmake_minimum_required (VERSION 3.8)

project("DonHugo")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(
	DonHugo
	"src/Application.cpp"
	"src/MicPlayback.cpp"
	"src/MicPlayback2.cpp"
	"src/Sound2.cpp"
	"src/Effects/Chorus.cpp"
	"src/Effects/Compressor.cpp"
	"src/Effects/Distortion.cpp"
	"src/Effects/Echo.cpp"
	"src/Effects/Effect.cpp"
	"src/Effects/EffectList.cpp"
	"src/Effects/Equalizer.cpp"
	"src/Effects/Flanger.cpp"
	"src/Effects/Gargle.cpp"
	"src/Effects/Reverb.cpp"
	"src/future/CaptureBuffer.cpp"
	"src/future/CaptureDevice.cpp"
	"src/future/SaveFile.cpp"
	"src/future/SoundBuffer.cpp"
	"src/future/SoundDevice.cpp"
	"src/future/Tools.cpp"
)

target_include_directories(
	DonHugo PUBLIC
	"${CMAKE_SOURCE_DIR}/Marx/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor/spdlog/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor"
)

target_link_libraries(
	DonHugo PUBLIC 
	Marx
	DSound
)

if (WIN32)
	target_compile_options(
		DonHugo PUBLIC 
		"-DMX_PLATFORM_WINDOWS"
	)
endif()