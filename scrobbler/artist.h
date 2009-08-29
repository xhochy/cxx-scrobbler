#ifndef __SCROBBLER_ARTIST_H_
#define __SCROBBLER_ARTIST_H_

#include <string>

// LibXML2
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace Scrobbler {
  class Artist {
  public:
    void writeXml(xmlTextWriterPtr writer) const;
    static Artist parse(xmlNodePtr node);
    int Playcount() const;
    std::string Name() const; 
    std::string Url() const;
  private:
    bool m_streamable;
    int m_playcount;
    int m_chartposition;
    int m_match;
    int m_tagcount;
    int m_rank;
    std::string m_mbid;
    std::string m_url;
    std::string m_name;
    std::string m_image_small;
    std::string m_image_medium;
    std::string m_image_large;
  };
} // Scrobbler

#endif // __SCROBBLER_ARTIST_H_
