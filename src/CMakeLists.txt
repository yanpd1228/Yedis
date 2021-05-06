project(src)

include_directories(
	../net
	../base
	)
aux_source_directory(. src_src)

add_executable(src ${src_src})

target_link_libraries(src net base)
