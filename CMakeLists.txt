# root source directory of the project ${CppQLite_SOURCE_DIR}
# root binary directory of the project ${CppQLite_BINARY_DIR}

# BUILD_SHARED_LIBS is CMAKE variable, shown her for clarity
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)
option(BUILD_TEST "Build all of CppQLite unit tests." OFF)

# Control CMAKE minimum version
cmake_minimum_required(VERSION 2.8.11)

# Define project name and language
project(CppQLite)

# Set compiler flags for targets
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "-std=c++11")
ELSEIF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET (CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
ELSEIF (MSVC)
  # On by default
ENDIF ()

link_directories(${SQLiteCppWrapper_BINARY_DIR}/src)

IF(BUILD_SHARED_LIBS)
  SET(SCppQLite_LIBRARY_TYPE SHARED)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
ELSE(BUILD_SHARED_LIBS)
  SET(CppQLite_LIBRARY_TYPE STATIC)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
ENDIF(BUILD_SHARED_LIBS)

# Create list of CppQLite source files
file(GLOB SOURCES "src/*.cpp")

add_library(CppQLite ${SOURCES})

target_include_directories(CppQLite PUBLIC ${CppQLite_SOURCE_DIR}/include)

# optional build test
IF(BUILD_TEST)
    enable_testing()
    
    # Create main.cpp which uses gtest
    file(WRITE test/src/main.cpp "#include \"gtest/gtest.h\"\n\n")
    file(APPEND test/src/main.cpp "int main(int argc, char **argv) {\n")
    file(APPEND test/src/main.cpp "  testing::InitGoogleTest(&argc, argv);\n")
    file(APPEND test/src/main.cpp "  return RUN_ALL_TESTS();\n")
    file(APPEND test/src/main.cpp "}\n")

    # Enable ExternalProject CMake module
    include(ExternalProject)

    # Set the build type if it isn't already
    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Release)
    endif()

    # Set default ExternalProject root directory
    set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

    # Download and install GoogleTest
    ExternalProject_Add(
        gtest
        SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        TIMEOUT 30
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/test/gtest
        # Disable install step
        INSTALL_COMMAND ""
        # Force separate output paths for debug and release builds to allow easy
        # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
                   -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
                   -Dgtest_force_shared_crt=ON
        # Wrap download, configure and build steps in a script to log output
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON)

    # Set gtest properties
    ExternalProject_Get_Property(gtest source_dir binary_dir)
    set_target_properties(gtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/gtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    #    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
    )

    ExternalProject_Get_Property(gtest source_dir)
    include_directories("${source_dir}/include")
    message("${source_dir}/include")

    # Add test executable target
    add_executable(MainTest ${PROJECT_SOURCE_DIR}/test/src/main.cpp
                            ${PROJECT_SOURCE_DIR}/test/src/unittest_SQLiteDatabase.cpp
                            ${PROJECT_SOURCE_DIR}/test/src/unittest_SQLiteOpenHelper.cpp
                            ${PROJECT_SOURCE_DIR}/test/src/SQLiteDatabaseHelper.cpp)

    # Create dependency of MainTest on googletest
    add_dependencies(MainTest gtest)

    # Specify MainTest's link libraries
    ExternalProject_Get_Property(gtest binary_dir)
    IF(MSVC)
        IF(BUILD_SHARED_LIBS)
            set(Suffix ".dll")
        ELSE(BUILD_SHARED_LIBS)
            set(Suffix ".lib")
        ENDIF(BUILD_SHARED_LIBS)
    ELSE(MSVC)
        IF(BUILD_SHARED_LIBS)
            set(Suffix ".so")
        ELSE(BUILD_SHARED_LIBS)
            set(Suffix ".a")
        ENDIF(BUILD_SHARED_LIBS)
        set(Pthread "-pthread")
    ENDIF(MSVC)

    # link required libraries
    target_link_libraries(MainTest CppQLite)
    target_link_libraries(MainTest sqlite3)

    target_link_libraries(
        MainTest
        debug ${binary_dir}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
        optimized ${binary_dir}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${Suffix}
        ${Pthread})
    
    add_test(all MainTest)
ENDIF(BUILD_TEST)





