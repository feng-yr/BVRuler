# The mini version of BVRuler dependencies
set(MIN_VER_CMAKE 2.8.12.2)
set(MIN_VER_OPENCV 2.4.0)

# These lists are later turned into target properties on BVRuler target
set(BVRuler_LINKER_LIBS "")
set(BVRuler_INCLUDE_DIRS "")
set(BVRuler_DEFINITIONS "")
set(BVRuler_COMPILE_OPTIONS "")

# ---[ OpenCV
find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs)
if(NOT OpenCV_FOUND) # if not OpenCV 3.x, then imgcodecs are not found
	find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc)
endif()
list(APPEND BVRuler_INCLUDE_DIRS PUBLIC ${OpenCV_INCLUDE_DIRS})
list(APPEND BVRuler_LINKER_LIBS PUBLIC ${OpenCV_LIBS})
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})")
