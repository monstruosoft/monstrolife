CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (life)
INCLUDE (FindPkgConfig)

OPTION (WANT_DEBUG "Build the project using debugging code" OFF)

SET (BASE_DIRECTORY .)
SET (SOURCE_DIR ${BASE_DIRECTORY}/src)
SET (FONT_DIR ${BASE_DIRECTORY}/font)
SET (CMAKE_C_FLAGS "-std=gnu99 -fgnu89-inline -no-pie -pg -g")
PKG_CHECK_MODULES (ALLEGRO5 allegro-5 allegro_memfile-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5)

INCLUDE_DIRECTORIES (${ALLEGRO5_INCLUDE_DIRS} ${BASE_DIRECTORY}/include)
LINK_DIRECTORIES (${ALLEGRO5_LIBRARY_DIRS})

ADD_EXECUTABLE (main ${SOURCE_DIR}/main.c)
TARGET_LINK_LIBRARIES(main ${ALLEGRO5_LIBRARIES} m)
FILE (COPY ${FONT_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

