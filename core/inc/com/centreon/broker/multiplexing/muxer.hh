/*
** Copyright 2009-2013,2015 Merethis
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
    void                statistics(io::properties& tree) const;
    void                wake();
    unsigned int        write(misc::shared_ptr<io::data> const& d);

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
