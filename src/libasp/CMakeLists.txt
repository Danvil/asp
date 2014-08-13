add_library(libasp SHARED
	asp.cpp
	slic.cpp
	pds/pds.cpp
	pds/FloydSteinberg.cpp
)

set_target_properties(libasp PROPERTIES OUTPUT_NAME asp)

include_directories(
)

target_link_libraries(libasp
	boost_thread
)
