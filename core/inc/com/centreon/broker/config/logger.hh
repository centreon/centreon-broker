/*
** Copyright 2009-2012 Merethis
**
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
*/

#ifndef CCB_CONFIG_LOGGER_HH
#  define CCB_CONFIG_LOGGER_HH

#  include <QString>
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
    void               level(logging::level l) throw ();
    logging::level     level() const throw ();
    void               max_size(unsigned long long max) throw ();
    unsigned long long max_size() const throw ();
    void               name(QString const& s);
    QString const&     name() const throw ();
    void               type(logger_type lt) throw ();
    logger_type        type() const throw ();
    void               types(unsigned int t) throw ();
    unsigned int       types() const throw ();

  private:
    void               _internal_copy(logger const& l);

    int                _facility;
    logging::level     _level;
    unsigned long long _max_size;
    QString            _name;
    logger_type        _type;
    unsigned int       _types;
  };
}

CCB_END()

#endif // !CCB_CONFIG_LOGGER_HH
