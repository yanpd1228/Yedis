project(src)

include_directories(
	../net
	../base
	)

set(src_src main.cpp)

add_executable(src ${src_src})

target_link_libraries(src net base)
