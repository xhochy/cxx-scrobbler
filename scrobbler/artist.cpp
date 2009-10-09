#include "artist.h"

// Boost
#include <boost/format.hpp>

using namespace std;
using namespace Scrobbler;

const char * getNodeContent(xmlNodePtr node) 
{
  if (node->children == NULL)
    return "";
  if (node->children->content == NULL)
    return "";
  return reinterpret_cast<const char *>(node->children->content);
}

Artist Artist::parse(xmlNodePtr node)
{
  Artist result;
  for (xmlNodePtr cur_node = node->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("playcount")))
      result.m_playcount = atoi(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, BAD_CAST "chartposition"))
      result.m_chartposition = atoi(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("rank")))
      result.m_chartposition = atoi(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("tagcount")))
      result.m_tagcount = atoi(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("match")))
      result.m_match = atoi(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("mbid")))
      result.m_mbid = string(getNodeContent(cur_node));
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("url")))
      result.m_url = getNodeContent(cur_node);
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("name")))
      result.m_name = getNodeContent(cur_node);
    else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("streamable"))) {
      string streamable(reinterpret_cast<const char *>(cur_node->children->content));
      result.m_streamable = (streamable == "1" || streamable == "true");      
    } else if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("image"))) {
      string size(reinterpret_cast<const char *>(xmlGetProp(cur_node, 
        reinterpret_cast<const xmlChar *>("size"))));
      if (size == "small") 
        result.m_image_small = string(getNodeContent(cur_node));
      else if (size == "medium") 
        result.m_image_medium = string(getNodeContent(cur_node));
      else if (size == "large")
        result.m_image_large = string(getNodeContent(cur_node));
    }
  }
  if (xmlHasProp(node, reinterpret_cast<const xmlChar *>("name")))
    result.m_name = reinterpret_cast<const char *>(xmlGetProp(node, 
      reinterpret_cast<const xmlChar *>("name")));
  if (xmlHasProp(node, reinterpret_cast<const xmlChar *>("rank")))
    result.m_rank = atoi(reinterpret_cast<const char *>(xmlGetProp(node, 
      reinterpret_cast<const xmlChar *>("rank"))));
  return result;
}

void Artist::writeXml(xmlTextWriterPtr writer) const
{
  // <artist>
  if (xmlTextWriterStartElement(writer, BAD_CAST "artist") < 0) 
    throw runtime_error("Error at xmlTextWriterStartElement");
    
  // playcount
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "playcount", "%d", 
    this->m_playcount) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // chartposition
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "chartposition", "%d",
    this->m_chartposition) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // rank
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "rank", "%d", 
    this->m_rank) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // tagcount
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "tagcount", "%d", 
    this->m_tagcount) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");
  
  // match
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "match", "%d", 
    this->m_match) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");
  
  // mbid
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "mbid", "%s", 
    this->m_mbid.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // url
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "url", "%s", 
    this->m_url.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // name
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "name", "%s", 
    this->m_name.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");

  // streamable
  if (xmlTextWriterWriteFormatElement(writer, BAD_CAST "streamable", "%s", 
    (this->m_streamable ? "true" : "false")) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatElement");
    
  // image "small"
  if (xmlTextWriterStartElement(writer, BAD_CAST "image") < 0) 
    throw runtime_error("Error at xmlTextWriterStartElement");
  if (xmlTextWriterWriteAttribute(writer, BAD_CAST "size", BAD_CAST "small") < 0)
    throw runtime_error("Error at xmlTextWriterWriteAttribute");
  if (xmlTextWriterWriteFormatString(writer, "%s", this->m_image_small.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatString");
  if (xmlTextWriterEndElement(writer) < 0) 
    throw runtime_error("Error at xmlTextWriterEndElement");

  // image "medium"
  if (xmlTextWriterStartElement(writer, BAD_CAST "image") < 0) 
    throw runtime_error("Error at xmlTextWriterStartElement");
  if (xmlTextWriterWriteAttribute(writer, BAD_CAST "size", BAD_CAST "medium") < 0)
    throw runtime_error("Error at xmlTextWriterWriteAttribute");
  if (xmlTextWriterWriteFormatString(writer, "%s", this->m_image_medium.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatString");
  if (xmlTextWriterEndElement(writer) < 0) 
    throw runtime_error("Error at xmlTextWriterEndElement");

  // image "large"
  if (xmlTextWriterStartElement(writer, BAD_CAST "image") < 0) 
    throw runtime_error("Error at xmlTextWriterStartElement");
  if (xmlTextWriterWriteAttribute(writer, BAD_CAST "size", BAD_CAST "large") < 0)
    throw runtime_error("Error at xmlTextWriterWriteAttribute");
  if (xmlTextWriterWriteFormatString(writer, "%s", this->m_image_large.c_str()) < 0)
    throw runtime_error("Error at xmlTextWriterWriteFormatString");
  if (xmlTextWriterEndElement(writer) < 0) 
    throw runtime_error("Error at xmlTextWriterEndElement");

  // </artist>
  if (xmlTextWriterEndElement(writer) < 0) 
    throw runtime_error("Error at xmlTextWriterEndElement");
}

string Artist::Name() const
{
  return this->m_name;
}

string Artist::Url() const
{
  return this->m_url;
}

int Artist::Playcount() const
{
  return this->m_playcount;
}
