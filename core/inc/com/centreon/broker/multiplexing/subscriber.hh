/*
** Copyright 2009-2013,2015 Centreon
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

#ifndef CCB_MULTIPLEXING_SUBSCRIBER_HH
#  define CCB_MULTIPLEXING_SUBSCRIBER_HH

#  include <ctime>
#  include <memory>
#  include <QMutex>
#  include <QQueue>
#  include <QString>
#  include <QWaitCondition>
#  include <set>
#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               multiplexing {
  /**
   *  @class subscriber subscriber.hh "com/centreon/broker/multiplexing/subscriber.hh"
   *  @brief Receive events from publishers and make them
   *         available through the interface::source interface.
   *
   *  This class is used as a cornerstone in event multiplexing.
   *  Each output willing to receive events will request a
   *  subscriber object. All publisher objects broadcast events
   *  they receive to every subscriber objects.
   *
   *  @see publisher
   */
  class                 subscriber : public io::stream {
  public:
                        subscriber(QString const& temporary_name = "");
                        ~subscriber();
    static void         event_queue_max_size(unsigned int max) throw ();
    static unsigned int event_queue_max_size() throw ();
    void                process(bool in = false, bool out = true);
    void                read(misc::shared_ptr<io::data>& d);
    void                read(
                          misc::shared_ptr<io::data>& d,
                          time_t timeout,
                          bool* timed_out = NULL);
    void                set_filters(
                          std::set<unsigned int> const& filters);
    void                statistics(io::properties& tree) const;
    unsigned int        write(misc::shared_ptr<io::data> const& d);

  private:
                        subscriber(subscriber const& s);
    subscriber&         operator=(subscriber const& s);
    void                clean();
    bool                _get_event_from_temporary(
                          misc::shared_ptr<io::data>& event);
    void                _get_last_event(
                          misc::shared_ptr<io::data>& event);

    QWaitCondition      _cv;
    QQueue<misc::shared_ptr<io::data> >
                        _events;
    static unsigned int _event_queue_max_size;
    std::set<unsigned int>
                        _filters;
    mutable QMutex      _mutex;
    bool                _process_in;
    bool                _process_out;
    bool                _recovery_temporary;
    misc::shared_ptr<io::stream>
                        _temporary;
    QString             _temporary_name;
    unsigned int        _total_events;
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_SUBSCRIBER_HH
