file(GLOB BENCHMARK_SRC "*.cpp")
project(benchmark)

add_executable(benchmark ${BENCHMARK_SRC})
target_link_libraries(benchmark PRIVATE utils)
install(TARGETS benchmark DESTINATION ${MATMUL_INSTALL_BIN_DIR})
