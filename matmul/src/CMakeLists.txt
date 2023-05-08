# add_subdirectory(cpu)
# add_subdirectory(gpu)
set(SOURCE_FILES baseline.cc)

add_executable(baseline ${SOURCE_FILES})
# target_link_libraries(baseline rand)
install(TARGETS baseline DESTINATION ${MATMUL_INSTALL_BIN_DIR})
