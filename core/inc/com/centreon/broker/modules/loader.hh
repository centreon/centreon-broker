/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_MODULES_LOADER_HH
#  define CCB_MODULES_LOADER_HH

#  include <QHash>
#  include <QString>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/modules/handle.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace    modules {
  /**
   *  @class loader loader.hh "com/centreon/broker/modules/loader.hh"
   *  @brief Load Centreon Broker plugins.
   *
   *  This class is used to load Centreon Broker plugins and hold
   *  reference to them as long as they exist.
   */
  class      loader {
  public:
    typedef QHash<QString, misc::shared_ptr<handle> >::iterator
             iterator;

             loader();
             loader(loader const& l);
             ~loader();
    loader&  operator=(loader const& l);
    iterator begin();
    iterator end();
    void     load_dir(
               QString const& dirname,
               void const* arg = NULL);
    void     load_file(
               QString const& filename,
               void const* arg = NULL);
    void     unload();

  private:
    QHash<QString, misc::shared_ptr<handle> >
             _handles;
  };
}

CCB_END()

#endif // !CCB_MODULES_LOADER_HH
