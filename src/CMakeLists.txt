project(src)

include_directories(
	../net
	../base
	)
aux_source_directory(. src_src)
#set(src_src main.cpp)

add_executable(src ${src_src})

target_link_libraries(src net base)
