add_executable(asp main.cpp)

add_definitions(-DSLIMAGE_IO_OPENCV)

include_directories(
)

target_link_libraries(asp
	libasp
	opencv_core
	opencv_highgui
	boost_program_options
	boost_system
)

