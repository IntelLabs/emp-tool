cmake_minimum_required (VERSION 2.8.11)
project (emptool)
set(NAME "emp-tool")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	if(NOT DEFINED OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")	
		message(STATUS "OPENSSL_ROOT_DIR set to default: ${OPENSSL_ROOT_DIR}")
	endif()
endif()
include(cmake/common.cmake)
include(cmake/source_of_randomness.cmake)
include(cmake/threading.cmake)


find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GMP_INCLUDE_DIR})


# Installation
set(sources
emp-tool/emp-tool.cpp
)

add_library(${NAME} SHARED ${sources})

install(DIRECTORY emp-tool DESTINATION include/)
install(DIRECTORY cmake/ DESTINATION cmake/)
install(TARGETS ${NAME} DESTINATION lib)
