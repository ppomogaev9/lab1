cmake_minimum_required(VERSION 3.10)

project(HashTable VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
	HashTable
	main.cpp
	hash_table.cpp
	miscellaneous.cpp
	randomize.cpp
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
	HashTableTests
	hash_table.cpp
	miscellaneous.cpp
	randomize.cpp
	test.cpp
)

target_link_libraries(
	HashTableTests
	gtest_main
)

if ( CMAKE_COMPILER_IS_GNUCC )
	target_compile_options(HashTableTests PRIVATE "-Wall")
endif()
if ( MSVC )
	target_compile_options(HashTableTests PRIVATE "/W4")
endif()

include(GoogleTest)
gtest_discover_tests(HashTableTests)
