/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_MULTIPLEXING_MUXER_HH
#  define CCB_MULTIPLEXING_MUXER_HH

#  include <memory>
#  include <queue>
#  include <QWaitCondition>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               multiplexing {
  /**
   *  @class muxer muxer.hh "com/centreon/broker/multiplexing/muxer.hh"
   *  @brief Receive and send events from/to the multiplexing engine.
   *
   *  This class is a cornerstone in event multiplexing. Each endpoint
   *  willing to communicate with the multiplexing engine will create a
   *  new muxer object. This objects broadcast events sent to it to all
   *  other muxer objects.
   *
   *  @see engine
   */
  class                 muxer : public io::stream {
  public:
    typedef             uset<unsigned int>
                        filters;

                        muxer(
                          std::string const& name,
                          bool persistent = false);
                        ~muxer();
    static void         event_queue_max_size(unsigned int max) throw ();
    static unsigned int event_queue_max_size() throw ();
    void                publish(misc::shared_ptr<io::data> const& d);
    bool                read(
                          misc::shared_ptr<io::data>& d,
                          time_t deadline);
    void                set_read_filters(filters const& fltrs);
    void                set_write_filters(filters const& fltrs);
    filters const&      get_read_filters() const;
    filters const&      get_write_filters() const;
    unsigned int        get_event_queue_size() const;
    void                statistics(io::properties& tree) const;
    void                wake();
    int                 write(misc::shared_ptr<io::data> const& d);

    static std::string  memory_file(std::string const& name);
    static std::string  queue_file(std::string const& name);

  private:
                        muxer(muxer const& other);
    muxer&              operator=(muxer const& other);
    void                _clean();
    bool                _get_event_from_temporary(
                          misc::shared_ptr<io::data>& event);
    void                _get_last_event(
                          misc::shared_ptr<io::data>& event);
    std::string         _memory_file() const;
    std::string         _queue_file() const;

    QWaitCondition      _cv;
    std::queue<misc::shared_ptr<io::data> >
                        _events;
    static unsigned int _event_queue_max_size;
    mutable QMutex      _mutex;
    std::string         _name;
    bool                _persistent;
    filters             _read_filters;
    std::auto_ptr<io::stream>
                        _temporary;
    unsigned int        _total_events;
    filters             _write_filters;
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_MUXER_HH
