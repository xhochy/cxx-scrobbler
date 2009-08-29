include_directories(${CXXSCROBBLER_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

FILE(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${headers} DESTINATION include)

add_library (scrobbler base.cpp library.cpp artist.cpp) 
INSTALL(TARGETS scrobbler DESTINATION lib)

