include_directories(${CXXSCROBBLER_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR})

add_library (scrobbler base.cpp library.cpp artist.cpp) 
