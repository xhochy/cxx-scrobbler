#include "library.h"

// C++/STL

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdio>

// Boost
#include <boost/format.hpp>

// cURL
#include <curl/curl.h>

// LibXML2
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;
using namespace Scrobbler;

Library::Library(std::string username)
{
  this->username = username;
}

Library::~Library()
{
}


namespace {
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)buffer, size*nmemb);
  return size * nmemb;
}
}

std::vector<Artist> Library::artists()
{
  std::vector<Artist> result;

  // Initialize cURL and request first page
  CURL * curl_handle = curl_easy_init();
  if (curl_handle == NULL)
    throw runtime_error("Could not initialize cURL.");
  std::string * outstr = new std::string();
  std::string url = "http://ws.audioscrobbler.com:80/2.0/?method=library.getartists";
  url += "&user=" + this->username + "&api_key=" + scobbler_api_key;
  if (curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str()) != CURLE_OK)
    throw runtime_error("Could not set URL in cURL request.");
  if (curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data) != CURLE_OK)
    throw runtime_error("Could not associate write function to cURL request.");
  if (curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outstr) != CURLE_OK)
    throw runtime_error("Could not associate write-pointer to cURL request.");
  if (curl_easy_perform(curl_handle) != CURLE_OK)
    throw runtime_error("Could not request library from Last.fm.");

  // parse result
  int pages = 1;
  xmlDocPtr doc = xmlReadDoc(BAD_CAST outstr->c_str(), NULL, NULL, 0);
  if (doc == NULL)
    throw runtime_error("Could not parse Last.fm response.");

  // Cleanup the cURL request
  delete outstr;

  // Iterate through the document
  xmlNodePtr root = xmlDocGetRootElement(doc);
  if (root == NULL)
    throw runtime_error("Could not select the root element of an Last.fm response.");
  for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, BAD_CAST "artists")) {
      // found artists node, get number of pages
      pages = atoi(reinterpret_cast<const char *>(xmlGetProp(cur_node, 
          BAD_CAST "totalPages")));

      // request all other pages
      // parse artists in this page
      for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
        if (xmlStrEqual(art_node->name, BAD_CAST "artist")) {
          result.push_back(Artist::parse(art_node));
        }
      }
      break;
    }
  }
  xmlFreeDoc(doc);

  // Accumulate all artists
  for (int i = 1; i < pages; i++) {
    if (curl_easy_setopt(curl_handle, CURLOPT_URL, (boost::format("%1%&page=%2%")
    % url % (i + 1)).str().c_str()) != CURLE_OK)
      throw runtime_error("Could not set URL in cURL request.");
    outstr = new std::string();
    if (curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outstr) != CURLE_OK)
      throw runtime_error("Could not associate write-pointer to cURL request.");
    if (curl_easy_perform(curl_handle) != CURLE_OK)
      throw runtime_error("Could not request library from Last.fm.");

    if ((doc = xmlReadDoc(BAD_CAST outstr->c_str(), NULL, NULL, 0)) == NULL)
      throw runtime_error("Could not parse Last.fm response.");
    // Iterate through the document
    root = xmlDocGetRootElement(doc);
    if (root == NULL)
      throw runtime_error("Could not select the root element of an Last.fm response.");
    for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
      if (xmlStrEqual(cur_node->name, BAD_CAST "artists")) {
        // parse artists in this page
        for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
          if (xmlStrEqual(art_node->name, BAD_CAST "artist")) {
            result.push_back(Artist::parse(art_node));
          }
        }
        break;
      }
    }

    // Cleanup
    xmlFreeDoc(doc);
    delete outstr;
  }

  // Cleanup everthing else
  curl_easy_cleanup(curl_handle);
  xmlCleanupParser();

  return result;
}
