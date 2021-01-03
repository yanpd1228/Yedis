project(base)

aux_source_directory(. base_src)
include_directories(
    ../base
	../net
	)




add_library(base ${base_src}) 
