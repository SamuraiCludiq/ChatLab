set(INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/common
                  ${PROJECT_SOURCE_DIR}/src/compatibility")

include_directories(${INCLUDE_DIRS})

if(BUILD_CLIENT)
    add_subdirectory(frontend)
endif()
if(BUILD_SERVER)
    add_subdirectory(server)
endif()
