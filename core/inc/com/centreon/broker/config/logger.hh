/*
** Copyright 2009-2012,2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_CONFIG_LOGGER_HH
#  define CCB_CONFIG_LOGGER_HH

#  include <string>
#  include "com/centreon/broker/logging/defines.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              config {
  /**
   *  @class logger logger.hh "com/centreon/broker/config/logger.hh"
   *  @brief Parameters of a logger object.
   *
   *  The logger class reflects parameters of logging objects of the
   *  logging namespace.
   */
  class                logger {
  public:
    enum               logger_type {
      unknown = 0,
      file,
      monitoring,
      standard,
      syslog
    };

                       logger();
                       logger(logger const& l);
                       ~logger();
    logger&            operator=(logger const& l);
    bool               operator==(logger const& l) const;
    bool               operator!=(logger const& l) const;
    bool               operator<(logger const& l) const;
    void               config(bool c) throw ();
    bool               config() const throw ();
    void               debug(bool d) throw ();
    bool               debug() const throw ();
    void               error(bool e) throw ();
    bool               error() const throw ();
    void               facility(int f) throw ();
    int                facility() const throw ();
    void               info(bool i) throw ();
    bool               info() const throw ();
    void               perf(bool i) throw ();
    bool               perf() const throw ();
    void               level(logging::level l) throw ();
    logging::level     level() const throw ();
    void               max_size(unsigned long long max) throw ();
    unsigned long long max_size() const throw ();
    void               name(std::string const& s);
    std::string const&     name() const throw ();
    void               type(logger_type lt) throw ();
    logger_type        type() const throw ();
    void               types(unsigned int t) throw ();
    unsigned int       types() const throw ();

  private:
    void               _internal_copy(logger const& l);

    int                _facility;
    logging::level     _level;
    unsigned long long _max_size;
    std::string        _name;
    logger_type        _type;
    unsigned int       _types;
  };
}

CCB_END()

#endif // !CCB_CONFIG_LOGGER_HH
