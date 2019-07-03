/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCB_VERSION_HH
#  define CCB_VERSION_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

#  define CENTREON_BROKER_VERSION_INCLUDE "19.04.0"

namespace            version {
  // Compile-time values.
  unsigned int const major = 19;
  unsigned int const minor = 04;
  unsigned int const patch = 0;
  char const* const  string = "19.04.0";
}

CCB_END()

#endif // !CCB_VERSION_HH
