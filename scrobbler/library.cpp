#include "library.h"

// C++/STL

#include <stdexcept>
#include <cstdio>

// Boost
#include <boost/format.hpp>

// cURL
#include <curl/curl.h>

// LibXML2
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace Scrobbler;

Library::Library(std::string username)
{
  this->username = username;
}

Library::~Library()
{
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)buffer, size*nmemb);
  return size * nmemb;
}

struct __library_params {
  std::string url;
  std::vector<Artist> artists;
  pthread_t thread;
};

static void * get_library_page(void * args)
{
  __library_params * params = (__library_params *) args;
  CURL * curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, params->url.c_str());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data); 
  std::string * outstr = new std::string();
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outstr); 
  curl_easy_perform(curl_handle);  
  
  xmlDocPtr doc = xmlReadDoc(reinterpret_cast<const xmlChar *>(outstr->c_str()), NULL, NULL, 0);
  // Iterate through the document
  xmlNodePtr root = xmlDocGetRootElement(doc);
  for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artists"))) {
      // parse artists in this page
      for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
        if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artist"))) {
          params->artists.push_back(Artist::parse(art_node));
        }
      }
      break;
    }
  }
  xmlFreeDoc(doc);
  
  // Cleanup
  curl_easy_cleanup(curl_handle);
  delete outstr;
  return NULL;
}

std::vector<Artist> Library::artists()
{
  std::vector<Artist> result(50);
  
  // Init cURL and request first page
  CURL * curl_handle = curl_easy_init();
  std::string * outstr = new std::string();
  std::vector<__library_params *> params;
  params.push_back(NULL);
  std::string url = "http://ws.audioscrobbler.com:80/2.0/?method=library.getartists";
  url += "&user=" + this->username + "&api_key=" + scobbler_api_key;
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data); 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outstr); 
  curl_easy_perform(curl_handle);
  
  // parse result
  int pages = 1;
  xmlDocPtr doc = xmlReadDoc(reinterpret_cast<const xmlChar *>(outstr->c_str()), NULL, NULL, 0);
  
  // Cleanup the cURL request
  curl_easy_cleanup(curl_handle);
  delete outstr;
  
  // Iterate through the document
  xmlNodePtr root = xmlDocGetRootElement(doc);
  for (xmlNodePtr cur_node = root->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artists"))) {
      // found artists node, get number of pages
      pages = atoi(reinterpret_cast<const char *>(xmlGetProp(cur_node, 
        reinterpret_cast<const xmlChar *>("totalPages"))));
      // request all other pages
      for (int i = 1; i < pages; i++) {
        params.push_back(new __library_params);
        params[i]->artists = std::vector<Artist>(50);
        params[i]->url = (boost::format("%1%&page=%2%") % url % (i + 1)).str();
        pthread_create(&params[i]->thread, NULL, get_library_page, params[i]);
      }
      // parse artists in this page
      for (xmlNodePtr art_node = cur_node->children; art_node; art_node = art_node->next) {
        if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("artist"))) {
          result.push_back(Artist::parse(art_node));
        }
      }
      break;
    }
  }
  xmlFreeDoc(doc);
  
  // Accumulate all artists
  for (int i = 1; i < pages; i++) {
    pthread_join(params[i]->thread, NULL);
    result.insert(result.end(), params[i]->artists.begin(), params[i]->artists.end());
    // @TODO add params[i]->artists to result
  }
  
  // Cleanup everthing else
  for (std::vector<__library_params *>::const_iterator i = params.begin(); i != params.end(); ++i) {
    delete *i;
  }
  xmlCleanupParser();
  
  return result;
}
