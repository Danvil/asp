add_executable(asp main.cpp)

target_link_libraries(asp
	libasp
	opencv_core
	opencv_highgui
	boost_program_options
	boost_system
)

