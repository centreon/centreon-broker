/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#ifndef CONFIG_LOGGER_HH_
# define CONFIG_LOGGER_HH_

# include <string>
# include "logging/defines.hh"

namespace              config {
  /**
   *  @class logger logger.hh "config/logger.hh"
   *  @brief Parameters of a logger object.
   *
   *  The logger class reflects parameters of logging objects of the logging
   *  namespace.
   */
  class                logger {
   public:
    enum               logger_type {
      unknown = 0,
      file,
      standard,
      syslog
    };

   private:
    logging::level     _level;
    std::string        _name;
    logger_type        _type;
    unsigned int       _types;
    void               _internal_copy(logger const& l);

   public:
                       logger();
                       logger(logger const& l);
                       ~logger();
    logger&            operator=(logger const& l);
    bool               operator==(logger const& l) const;
    bool               operator!=(logger const& l) const;
    bool               operator<(logger const& l) const;
    void               config(bool c);
    bool               config() const;
    void               debug(bool d);
    bool               debug() const;
    void               error(bool e);
    bool               error() const;
    void               info(bool i);
    bool               info() const;
    void               level(logging::level l);
    logging::level     level() const;
    void               name(std::string const& s);
    std::string const& name() const;
    void               type(logger_type lt);
    logger_type        type() const;
    void               types(unsigned int t);
    unsigned int       types() const;
  };
}

#endif /* !CONFIG_LOGGER_HH_ */
