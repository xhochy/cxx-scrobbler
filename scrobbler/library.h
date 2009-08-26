// STL includes
#include <string>
#include <vector>

// Scrobbler includes
#include <scrobbler/artist.h>
#include <scrobbler/base.h>

namespace Scrobbler {
  class Library : public Base {
  public:
    Library(std::string username);
    virtual ~Library();
    std::vector<Artist> artists();
  private:
    std::string username;
  };
} // Scrobbler
