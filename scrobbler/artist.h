#include <string>

// LibXML2
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace Scrobbler {
  class Artist {
  public:
    static Artist parse(xmlNodePtr node);
  private:
    int m_playcount;
    std::string m_mbid;
    std::string m_url;
    std::string m_name;
  };
} // Scrobbler
