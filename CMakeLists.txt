PROJECT (icfp)

CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)

SET (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

IF (CMAKE_CXX_COMPILER MATCHES ".*clang")
  SET (CMAKE_COMPILER_IS_CLANGXX 1)
ENDIF ()

IF (CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-Wall -Wextra -Wstrict-null-sentinel -Woverloaded-virtual -Wnoexcept -Wuninitialized -Winit-self -Wformat=2)
    ADD_DEFINITIONS(-Wno-missing-field-initializers -Wno-non-template-friend -Wno-unused-parameter)

    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
    ADD_DEFINITIONS(-DSTAR_GCC_VISIBILITY)

    SET (CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG -fno-omit-frame-pointer")
    SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG -fno-omit-frame-pointer")

    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -DNDEBUG -g -fno-omit-frame-pointer")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -DNDEBUG -g -fno-omit-frame-pointer")
    
    OPTION (ENABLE_PROFILING "Enable profiling" OFF)
    IF (ENABLE_PROFILING)
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
    ENDIF (ENABLE_PROFILING)
ENDIF (CMAKE_COMPILER_IS_GNUCC)

IF (CMAKE_COMPILER_IS_CLANGXX)
    ADD_DEFINITIONS(-Wall -Wextra)
    ADD_DEFINITIONS(-Wno-missing-field-initializers -Wno-non-template-friend -Wno-unused-parameter)

    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
    ADD_DEFINITIONS(-DSTAR_GCC_VISIBILITY)

    SET (CMAKE_C_FLAGS_RELEASE "-O4 -ffast-math -DNDEBUG -fno-omit-frame-pointer")
    SET (CMAKE_CXX_FLAGS_RELEASE "-O4 -ffast-math -DNDEBUG -fno-omit-frame-pointer")

    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O4 -ffast-math -DNDEBUG -g -fno-omit-frame-pointer")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O4 -ffast-math -DNDEBUG -g -fno-omit-frame-pointer")
    
ENDIF (CMAKE_COMPILER_IS_CLANGXX)

SET (main_HEADERS
    )

SET (main_SOURCES
      main.cpp
    )

ADD_EXECUTABLE (icfp ${main_HEADERS} ${main_SOURCES})
