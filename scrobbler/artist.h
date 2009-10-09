#ifndef __SCROBBLER_ARTIST_H_
#define __SCROBBLER_ARTIST_H_

#include <string>

// LibXML2
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

namespace Scrobbler {
  /**
   * Class for artist information and invocation of artist related functions
   */
  class Artist {
  public:
	/**
	 * Writes the artist information into an XML document
	 */
	void writeXml(xmlTextWriterPtr writer) const;

	/**
	 * Reads the available data from a XML node to generate an artist object
	 * out of it.
	 */
    static Artist parse(xmlNodePtr node);

    /**
     * How often an artist was played. This information differs from which
     * source this artist object was extract.
     *
     * E.g. if this artist object is out of one user's library, playcount is
     * the number of times the user heard this artist.
     */
    int Playcount() const;

    /**
     * The name of the artist.
     */
    std::string Name() const; 

    /**
     * The URL to the artist's Last.fm page.
     */
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
