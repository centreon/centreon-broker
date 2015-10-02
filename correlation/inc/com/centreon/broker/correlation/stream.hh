/*
** Copyright 2015 Centreon
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

#ifndef CCB_CORRELATION_STREAM_HH
#  define CCB_CORRELATION_STREAM_HH

#  include <QString>
#  include <QMap>
#  include <QPair>
#  include "com/centreon/broker/correlation/node.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
class            persistent_cache;

namespace        correlation {
  /**
   *  @class stream stream.hh "com/centreon/broker/correlation/stream.hh"
   *  @brief Correlation stream.
   *
   *  Generate correlation events from a monitoring stream composed of
   *  various monitoring events (host/service statuses, downtimes,
   *  acknowledgements, ...).
   */
  class          stream : public io::stream {
  public:
                 stream(
                   QString const& correlation_file,
                   misc::shared_ptr<persistent_cache> cache
                   = misc::shared_ptr<persistent_cache>(),
                   bool load_correlation = true,
                   bool passive = false);
                 ~stream();
    bool         read(
                   misc::shared_ptr<io::data>& d,
                   time_t deadline);
    void         update();
    int          write(misc::shared_ptr<io::data> const& d);
    void         set_state(
                   QMap<QPair<unsigned int, unsigned int>, node> const& st);
    QMap<QPair<unsigned int, unsigned int>, node> const&
                 get_state() const;

  private:
                 stream(stream const& other);
    stream&      operator=(stream const& other);
    void         _load_correlation();
    void         _load_correlation_event(misc::shared_ptr<io::data> const& d);
    void         _save_persistent_cache();

    misc::shared_ptr<persistent_cache>
                 _cache;
    QString      _correlation_file;
    bool         _passive;

    std::auto_ptr<io::stream>
                 _pblsh;

    QMap<QPair<unsigned int, unsigned int>, node>
                 _nodes;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_STREAM_HH
