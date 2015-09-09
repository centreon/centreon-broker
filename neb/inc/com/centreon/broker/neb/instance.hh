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

#ifndef CCB_NEB_INSTANCE_HH
#  define CCB_NEB_INSTANCE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class instance instance.hh "com/centreon/broker/neb/instance.hh"
   *  @brief Information about Nagios process.
   *
   *  This class holds information about a Nagios process, like whether
   *  it is running or not, in daemon mode or not, ...
   */
  class            instance : public io::data {
  public:
                   instance();
                   instance(instance const& i);
                   ~instance();
    instance&      operator=(instance const& i);
    unsigned int   type() const;

    QString        engine;
    unsigned int   id;
    bool           is_running;
    QString        name;
    unsigned int   pid;
    timestamp      program_end;
    timestamp      program_start;
    QString        version;

  private:
    void           _internal_copy(instance const& i);
  };
}

CCB_END()

#endif // !CCB_NEB_INSTANCE_HH
