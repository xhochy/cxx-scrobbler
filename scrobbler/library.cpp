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
  xmlDocPtr doc = xmlReadDoc(reinterpret_cast<const xmlChar *>(outstr->c_str()), NULL, NULL, 0);
  
  // Cleanup the cURL request
  delete outstr;
  
  // Iterate through the document
  xmlNodePtr root = xmlDocGetRootElement(doc);
  for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artists"))) {
      // found artists node, get number of pages
      pages = atoi(reinterpret_cast<const char *>(xmlGetProp(cur_node, 
        reinterpret_cast<const xmlChar *>("totalPages"))));
      
      // request all other pages
      // parse artists in this page
      for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
        if (xmlStrEqual(art_node->name, reinterpret_cast<const xmlChar *>("artist"))) {
          result.push_back(Artist::parse(art_node));
        }
      }
      break;
    }
  }
  xmlFreeDoc(doc);
  
  // Accumulate all artists
  for (int i = 1; i < pages; i++) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, (boost::format("%1%&page=%2%") 
      % url % (i + 1)).str().c_str());
    outstr = new std::string();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outstr); 
    curl_easy_perform(curl_handle);  
    
    doc = xmlReadDoc(reinterpret_cast<const xmlChar *>(outstr->c_str()), NULL, NULL, 0);
    // Iterate through the document
    root = xmlDocGetRootElement(doc);
    for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
      if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artists"))) {
        // parse artists in this page
        for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
          if (xmlStrEqual(art_node->name, reinterpret_cast<const xmlChar *>("artist"))) {
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
