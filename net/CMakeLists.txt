project(net)


aux_source_directory(. net_src)

include_directories(
	../base
	)


add_library(net ${net_src})

target_link_libraries(net base)
