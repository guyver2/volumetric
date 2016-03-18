#include "volume.hpp"


std::ostream& operator<<(std::ostream &out, const shape &s) {
  return out << "(" << s.w << ", " << s.h << ", " << s.d << ")";
}
