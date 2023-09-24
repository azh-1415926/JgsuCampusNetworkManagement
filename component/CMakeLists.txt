file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.c include/*.h)
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
add_library(Component OBJECT ${srcs})
target_include_directories(Component PUBLIC .)
target_include_directories(Component PUBLIC ${ROOTDIR}/vendor/md5)

add_subdirectory(${ROOT_DIR}/vendor vendor_binnary_dir)
target_link_libraries(Component PRIVATE vendor)