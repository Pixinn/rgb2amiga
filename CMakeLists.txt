cmake_minimum_required (VERSION 3.12)
cmake_policy(SET CMP0074 NEW)

project (Rgb2Amiga)

# flags not compatibles with magick++
if(WIN32)
	string (REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	string (REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif(WIN32)

# The version number.
set (Rgb2Hires_VERSION_MAJOR 1)
set (Rgb2Hires_VERSION_MINOR 0)
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)

# dependencies
if(WIN32)
	# sdl2
	if(NOT DEFINED ENV{SDL2_HOME})
		message(FATAL_ERROR "Please set a SDL2_HOME environment variable to the root directory of SDL2")		
	endif()
	set(SDL2_DIR $ENV{SDL2_HOME})
	# imagemagik
	if(NOT DEFINED ENV{MAGICK_HOME})
		message(FATAL_ERROR "Please set a MAGICK_HOME environment variable to the root directory of ImageMagick6")		
	endif()
	find_package(ImageMagick HINTS $ENV{MAGICK_HOME} REQUIRED COMPONENTS Magick++)
else()
	find_package(ImageMagick 6.9 EXACT REQUIRED COMPONENTS Magick++ )
endif(WIN32)
find_package(SDL2 REQUIRED)

# directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

# libraries
# amivideo
set(AMIVIDEO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libamivideo/src/libamivideo)
set(AMIVIDEO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libamivideo/src)
add_library(amivideo ${AMIVIDEO_DIR}/palette.c ${AMIVIDEO_DIR}/screen.c ${AMIVIDEO_DIR}/viewportmode.c)
target_include_directories(amivideo PUBLIC ${AMIVIDEO_DIR})
target_compile_features(amivideo PUBLIC c_std_90)

# iff
set(IFF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libiff/src/libiff)
set(IFF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libiff/src)
add_library(iff 		${IFF_DIR}/cat.c
						${IFF_DIR}/chunk.c
						${IFF_DIR}/error.c
						${IFF_DIR}/extension.c
						${IFF_DIR}/form.c
						${IFF_DIR}/group.c
						${IFF_DIR}/id.c
						${IFF_DIR}/iff.c
						${IFF_DIR}/io.c
						${IFF_DIR}/list.c
						${IFF_DIR}/prop.c
						${IFF_DIR}/rawchunk.c
						${IFF_DIR}/util.c
)						
target_compile_features(iff PUBLIC c_std_90)

# ilbm
set(ILBM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libilbm/src/libilbm)
set(ILBM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libilbm/src)
add_library(ilbm 		${ILBM_DIR}/bitmapheader.c
						${ILBM_DIR}/byterun.c
						${ILBM_DIR}/colormap.c
						${ILBM_DIR}/colorrange.c
						${ILBM_DIR}/cycleinfo.c
						${ILBM_DIR}/destmerge.c
						${ILBM_DIR}/drange.c
						${ILBM_DIR}/grab.c
						${ILBM_DIR}/ilbm.c
						${ILBM_DIR}/ilbmimage.c
						${ILBM_DIR}/interleave.c
						${ILBM_DIR}/sprite.c
						${ILBM_DIR}/viewport.c
)						
target_compile_features(ilbm PUBLIC c_std_90)
target_include_directories(ilbm PRIVATE ${IFF_INCLUDE_DIR})

# 2Amiga
set(2AMIGA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib2Amiga)
set(2AMIGA_INCLUDE_DIR ${2AMIGA_DIR}/include)
add_library(2Amiga  ${2AMIGA_INCLUDE_DIR}/CError.h
					${2AMIGA_INCLUDE_DIR}/CAmigaImage.h
					${2AMIGA_INCLUDE_DIR}/CChunkyImage.h
					${2AMIGA_INCLUDE_DIR}/CPalette.h
					${2AMIGA_DIR}/src/CAmigaImage.cpp					
					${2AMIGA_DIR}/src/CChunkyImage.cpp					
					${2AMIGA_DIR}/src/CPalette.cpp
)
target_compile_definitions(2Amiga PRIVATE MAGICKCORE_QUANTUM_DEPTH=16 MAGICKCORE_HDRI_ENABLE=0)
target_include_directories(2Amiga PRIVATE 	${2AMIGA_DIR}/include
											${AMIVIDEO_INCLUDE_DIR}
											${IFF_INCLUDE_DIR}
											${ILBM_INCLUDE_DIR}
											${ImageMagick_INCLUDE_DIRS}
)

# Application
set(APP_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/app/cli/src)
add_executable(${PROJECT_NAME} ${APP_DIR}/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE  ${APP_DIR}/tclap
													${ImageMagick_INCLUDE_DIRS}
													${SDL2_INCLUDE_DIRS}
													${2AMIGA_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME} ${ImageMagick_LIBRARIES} ${SDL2_LIBRARIES} 2Amiga ilbm iff amivideo)    
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})

# Windows only: copy dlls
if(WIN32)
	set(DEBUG_EXE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_im_db_dlls.bat")
	set(RELEASE_EXE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_im_rl_dlls.bat")
	string (REPLACE "/" "\\" WIN_BIN_DIR "$<$<CONFIG:debug>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}>$<$<CONFIG:release>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}>")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND "$<$<CONFIG:debug>:${DEBUG_EXE_PATH}>$<$<CONFIG:release>:${RELEASE_EXE_PATH}>" ${WIN_BIN_DIR})
	
	message(">>>> ${ImageMagick_LIBRARIES}")
	
	# if(${CMAKE_INTDIR} MATCHES Debug)
		# message("debug")
		# string (REPLACE "/" "\\" WIN_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
		# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_im_db_dlls.bat ${WIN_BIN_DIR})
	# else()
		# message("release")
		# string (REPLACE "/" "\\" WIN_BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
		# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_im_rl_dlls.bat ${WIN_BIN_DIR})
	# endif()
endif(WIN32)