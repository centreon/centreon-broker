/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_LOGGING_ENGINE_HH
#  define CC_LOGGING_ENGINE_HH

#  include <climits>
#  include <vector>
#  include "com/centreon/concurrency/mutex.hh"
#  include "com/centreon/namespace.hh"

CC_BEGIN()

namespace                      logging {
  class                        backend;

  /**
   *  @class engine engine.hh "com/centreon/logging/engine.hh"
   *  @brief Logging object manager.
   *
   *  This is an external access point to logging system. Allow to
   *  register backends and write log message into them.
   */
  class                        engine {
  public:
    unsigned long              add(
                                 backend* obj,
                                 unsigned long long types,
                                 unsigned int verbose);
    /**
     *  Get the logger engine singleton.
     *
     *  @return The unique instance of logger engine.
     */
    static engine&             instance() throw () {
      return (*_instance);
    }

    /**
     *  Check if at least one backend can log with this parameter.
     *
     *  @param[in] flag     The logging type to log.
     *  @param[in] verbose  The verbosity level.
     *
     *  @return True if at least one backend can log with this
     *          parameter, otherwise false.
     */
    bool                       is_log(
                                 unsigned long long types,
                                 unsigned int verbose) const throw () {
      if (verbose >= sizeof(unsigned int) * CHAR_BIT)
        return (false);
      return (_list_types[verbose] & types);
    }
    static void                load();
    void                       log(
                                 unsigned long long types,
                                 unsigned int verbose,
                                 char const* msg,
                                 unsigned int size);
    bool                       remove(unsigned long id);
    unsigned int               remove(backend* obj);
    void                       reopen();
    static void                unload();

  private:
    struct                     backend_info {
      unsigned long            id;
      backend*                 obj;
      unsigned long long       types;
      unsigned int             verbose;
    };

                               engine();
                               engine(engine const& right);
                               ~engine() throw ();
    engine&                    operator=(engine const& right);
    void                       _rebuild_types();

    std::vector<backend_info*> _backends;
    unsigned long              _id;
    static engine*             _instance;
    unsigned long long         _list_types[sizeof(unsigned int) * CHAR_BIT];
    mutable concurrency::mutex _mtx;
  };
}

CC_END()

#endif // !CC_LOGGING_ENGINE_HH
