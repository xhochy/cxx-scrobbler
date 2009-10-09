// STL includes
#include <string>
#include <vector>

// Scrobbler includes
#include <scrobbler/artist.h>
#include <scrobbler/base.h>

namespace Scrobbler {
  class Library : public Base {
  public:
	/**
	 * Create a new library for a certain user.
	 */
    Library(std::string username);

    /**
     * destructor
     */
    virtual ~Library();

    /**
     * Get all artists that this user has heard.
     */
    std::vector<Artist> artists();
  private:
    std::string username;
  };
} // Scrobbler
