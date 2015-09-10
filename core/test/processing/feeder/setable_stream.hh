/*
** Copyright 2011-2013 Centreon
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

#ifndef CCE_SETABLE_STREAM_HH
#  define CCE_SETABLE_STREAM_HH

#  include <QList>
#  include "com/centreon/broker/io/stream.hh"

/**
 *  @class setable_stream setable_stream.hh
 *  @brief Setable stream.
 *
 *  Stream that can be set to generate errors or not.
 */
class             setable_stream : public com::centreon::broker::io::stream {
public:
                  setable_stream();
                  setable_stream(setable_stream const& ss);
                  ~setable_stream();
  setable_stream& operator=(setable_stream const& ss);
  unsigned int    get_count() const;
  QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const&
                  get_stored_events() const;
  void            process(bool in = false, bool out = true);
  void            read(
                    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data>& d);
  void            set_count(unsigned int cnt);
  void            set_replay_events(bool replay);
  void            set_sleep_time(unsigned int ms);
  void            set_store_events(bool store);
  unsigned int    write(
                    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> const& d);

private:
  void            _internal_copy(setable_stream const& ss);

  unsigned int    _count;
  bool            _process_in;
  bool            _process_out;
  bool            _replay_events;
  QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >
                  _replay;
  unsigned int    _sleep_time;
  bool            _store_events;
  QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >
                  _stored_events;
};

#endif // !CCE_SETABLE_STREAM_HH
