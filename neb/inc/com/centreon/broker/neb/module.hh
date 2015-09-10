/*
** Copyright 2009-2012 Centreon
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

#ifndef CCB_NEB_MODULE_HH
#  define CCB_NEB_MODULE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class module module.hh "com/centreon/broker/neb/module.hh"
   *  @brief Represents a module loaded in a Nagios instance.
   *
   *  The scheduling engine supports modules that extend its
   *  original features. This class describes such modules.
   */
  class            module : public io::data {
  public:
                   module();
                   module(module const& m);
                   ~module();
    module&        operator=(module const& m);
    unsigned int   type() const;

    QString        args;
    bool           enabled;
    QString        filename;
    bool           loaded;
    bool           should_be_loaded;

  private:
    void           _internal_copy(module const& m);
  };
}

CCB_END()

#endif // !CCB_NEB_MODULE_HH
