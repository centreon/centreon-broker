/*
** Copyright 2011 Centreon
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

#ifndef CCB_MULTIPLEXING_HOOKER_HH_
# define CCB_MULTIPLEXING_HOOKER_HH_

# include <QObject>
# include "com/centreon/broker/io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        multiplexing {
        /**
         *  @class hooker hooker.hh "com/centreon/broker/multiplexing/hooker.hh"
         *  @brief Hook object.
         *
         *  Place a hook on the multiplexing engine.
         */
        class          hooker : public QObject, public io::stream {
          Q_OBJECT

         protected:
          bool         _registered;

         public:
                       hooker();
                       hooker(hooker const& h);
          virtual      ~hooker();
          hooker&      operator=(hooker const &h);
          virtual void process(bool in = false, bool out = true);
          virtual void starting() = 0;
          virtual void stopping() = 0;
        };
      }
    }
  }
}

#endif /* !CCB_MULTIPLEXING_HOOKER_HH_ */
