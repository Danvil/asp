add_library(libasp SHARED
	asp.cpp
	algos/ASP.cpp
	algos/DASP.cpp
	algos/SLIC.cpp
	pds/pds.cpp
	pds/FloydSteinberg.cpp
)

set_target_properties(libasp PROPERTIES OUTPUT_NAME asp)

target_link_libraries(libasp
	boost_thread
)
