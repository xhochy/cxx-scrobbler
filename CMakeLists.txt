cmake_minimum_required (VERSION 2.6)
project (CXXSCROBBLER) 

find_path(LIBXML2_INCLUDE_DIR libxml/parser.h /usr/include/libxml2)
find_path(CURL_INCLUDE_DIR curl/curl.h /usr/include)

set(BUILD_SHARED_LIBS ON)
add_subdirectory(scrobbler)
