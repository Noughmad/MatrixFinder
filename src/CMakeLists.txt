set(WALDO_SRCS
	main.cpp
	matrixfinder.cpp
)
set(WALDO_TEST_SRCS
	test_main.cpp
	matrixfinder.cpp
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(waldo ${WALDO_SRCS})
target_link_libraries(waldo ${OpenCL_LIBRARIES})

add_executable(waldo_test ${WALDO_TEST_SRCS})
target_link_libraries(waldo_test ${OpenCL_LIBRARIES})

add_test(test_waldotest waldo_test ${CMAKE_SOURCE_DIR}/data/data.txt ${CMAKE_SOURCE_DIR}/data/pattern.txt ${CMAKE_SOURCE_DIR}/kernel/find.cl)


install(TARGETS waldo RUNTIME DESTINATION bin)
