cmake_minimum_required(VERSION 3.19)
project(contours VERSION 0.1)

#set(CMAKE_CXX_STANDARD 20)


string(TIMESTAMP BUILDDATE)
add_compile_options(-ggdb -Wno-deprecated-enum-enum-conversion -DVERSION="${CMAKE_PROJECT_VERSION}"  -DBUILDDATE="${BUILDDATE}")

add_executable(contours src/contours.cpp )

find_package(PkgConfig REQUIRED)

#Establishes the target install directory for any built libraries:
set(BUILD_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/external/usr")


#opencv:
if (DEFINED BUILD_OPENCV)
	if (DEFINED HOST)
		set(CMAKECMD "${HOST}-cmake")
	else()
		set(CMAKECMD "cmake")
	endif()
	set(OPENCV_DEPS -lpng -lz)
	set(CMAKE_ARGUMENTS  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENCV_GENERATE_PKGCONFIG=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_apps=OFF -DOPENCV_TRACE=OFF -DWITH_IPP=OFF -DBUILD_LIST=core,imgproc,imgcodecs -DWITH_OPENCL=OFF -DWITH_PNG=ON -DWITH_JPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_JPEGXL=OFF -DWITH_AVIF=OFF -DWITHV4L=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_MSMF=OFF -DWITH_DSHOW=OFF -DWITH_AVFOUNDATION=OFF -DVIDEOIO_ENABLE_PLUGINS=OFF -DWITH_GTK=OFF -DWITH_WIN32UI=OFF -DHIGHGUI_ENABLE_PLUGINS=OFF -DWITH_PROTOBUF=OFF -DBUILD_PROTOBUF=OFF -DOPENCV_DNN_OPENCL=OFF -DENABLE_PRECOMPILED_HEADERS=OFF -DENABLE_PYLINT=OFF -DENABLE+FLAKE8=OFF -DBUILD_JAVA=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DWITH_ITT=OFF -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )
	#set(CMAKE_ARGUMENTS  -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENCV_GENERATE_PKGCONFIG=ON -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_apps=OFF -DBUILD_LIST=core,imgproc,imgcodecs -DWITH_OPENCL=OFF -DWITH_PNG=ON -DWITH_JPEG=OFF -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_OPENJPEG=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_JPEGXL=OFF -DWITHV4L=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF -DWITH_MSMF=OFF -DWITH_DSHOW=OFF -DWITH_AVFOUNDATION=OFF -DVIDEOIO_ENABLE_PLUGINS=OFF -DWITH_GTK=OFF -DWITH_WIN32UI=OFF -DHIGHGUI_ENABLE_PLUGINS=OFF -DWITH_PROTOBUF=OFF -DBUILD_PROTOBUF=OFF -DOPENCV_DNN_OPENCL=OFF -DENABLE_PRECOMPILED_HEADERS=OFF -DENABLE_PYLINT=OFF -DENABLE+FLAKE8=OFF -DBUILD_JAVA=OFF -DBUILD_FAT_JAVA_LIB=OFF -DBUILD_opencv_python2=OFF -DBUILD_opencv_python3=OFF -DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX} )
	include(ExternalProject)
	if (BUILD_OPENCV STREQUAL "GITHUB")
		message(STATUS "Configuring opencv as an external project (GitHub clone).")
		ExternalProject_Add(opencv_download
			PREFIX external/opencv
			GIT_REPOSITORY	  https://github.com/opencv/opencv.git
			GIT_PROGRESS      ON
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	elseif (BUILD_OPENCV STREQUAL "SRCPKG")
		message(STATUS "Configuring opencv as an external project (URL download).")
		ExternalProject_Add(opencv_download
			PREFIX external/opencv
			#DOWNLOAD_DIR "external"
			URL https://github.com/opencv/opencv/archive/refs/heads/4.x.zip
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	elseif (EXISTS ${BUILD_OPENCV})
		message(STATUS "Configuring opencv as an external project (local file: ${BUILD_OPENCV}).")
		ExternalProject_Add(opencv_download
			URL ${BUILD_OPENCV}
			DOWNLOAD_EXTRACT_TIMESTAMP FALSE
			PREFIX external/opencv
			CMAKE_COMMAND	${CMAKECMD}
			CMAKE_ARGS ${CMAKE_ARGUMENTS}
			INSTALL_COMMAND make install
			UPDATE_COMMAND ""
		)
	else ()
		message(FATAL_ERROR "BUILD_OPENCV not valid: ${BUILD_OPENCV} (Either GITHUB, SRCPKG, or valid local path/srcpackage")
	endif()

	set(OPENCV_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/include)
	set(OPENCV_INC_DIR ${LIBNOISE_INCLUDE_DIR} CACHE STRING "opencv include directory")
	set(OPENCV_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib)
	set(OPENCV_LIB   ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib/libopencv_imgcodecs.a ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib/libopencv_imgproc.a  ${CMAKE_CURRENT_BINARY_DIR}/external/usr/lib/libopencv_core.a )
	
	add_library(opencv STATIC IMPORTED)
	set_target_properties(opencv PROPERTIES IMPORTED_LOCATION ${OPENCV_LIB_DIR} ) #"${OPENCV_LIB_DIR}/libopencv_imgproc.a  ${OPENCV_LIB_DIR}/libopencv_core.a" )
	set(OPENCV_FOUND TRUE)
	add_dependencies(contours opencv_download)
	
	target_include_directories(contours PRIVATE ${OPENCV_INCLUDE_DIR})
	target_link_libraries(contours ${OPENCV_LIB}  ${OPENCV_DEPS})
else()
	find_package( OpenCV REQUIRED )
	if (OpenCV_FOUND)
		include_directories( ${OpenCV_INCLUDE_DIRS} )
		target_link_libraries( contours ${OpenCV_LIBS} )
	else()
		message(FATAL_ERROR "OpenCV not found")
	endif(OpenCV_FOUND)
endif ()


if(WIN32)
#	target_link_libraries(contours wsock32 ws2_32 psapi)
endif(WIN32)

add_subdirectory(src)

