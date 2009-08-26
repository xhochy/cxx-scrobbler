#include "artist.h"

using namespace Scrobbler;

Artist Artist::parse(xmlNodePtr node)
{
  Artist result;
  
  for (xmlNodePtr cur_node = node->children; cur_node; cur_node = cur_node->next) {
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("playcount")))
      result.m_playcount = atoi(reinterpret_cast<const char *>(cur_node->content));
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("mbid")))
      result.m_mbid = std::string(reinterpret_cast<const char *>(cur_node->content));
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("url")))
      result.m_url = std::string(reinterpret_cast<const char *>(cur_node->content));
    if (xmlStrEqual(cur_node->name, reinterpret_cast<const xmlChar *>("name")))
      result.m_name = std::string(reinterpret_cast<const char *>(cur_node->content));
  }
  /* TODO
      data[:match] = child.content.to_i if child.name == 'match'
      data[:tagcount] = child.content.to_i if child.name == 'tagcount'
      data[:chartposition] = child.content if child.name == 'chartposition'
      if child.name == 'streamable'
        if ['1', 'true'].include?(child.content)
          data[:streamable] = true
        else
          data[:streamable] = false
        end
      end

      if child.name == 'image'
        data[:image_small] = child.content if child['size'] == 'small'
        data[:image_medium] = child.content if child['size'] == 'medium'
        data[:image_large] = child.content if child['size'] == 'large'
      end
    end 
    # If we have not found anything in the content of this node yet then
    # this must be a simple artist node which has the name of the artist
    # as its content
    data[:name] = xml.content if data == {}
    
    # Get all information from the root's attributes
    data[:name] = xml['name'] if xml['name']
    data[:rank] = xml['rank'].to_i if xml['rank']
    data[:streamable] = xml['streamable'] if xml['streamable']
    data[:mbid] = xml['mbid'] if xml['mbid']
  */
  return result;
}
