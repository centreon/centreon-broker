/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_SETABLE_ENDPOINT_HH
#  define CCB_SETABLE_ENDPOINT_HH

#  include <QList>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "test/processing/feeder/setable_stream.hh"

/**
 *  @class setable_endpoint setable_endpoint.hh
 *  @brief Setable endpoint.
 *
 *  Endpoint that can be set to generate errors or not.
 */
class               setable_endpoint : public com::centreon::broker::io::endpoint {
public:
                    setable_endpoint();
                    setable_endpoint(setable_endpoint const& se);
                    ~setable_endpoint();
  setable_endpoint& operator=(setable_endpoint const& se);
  com::centreon::broker::io::endpoint*
                    clone() const;
  void              close();
  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::stream>
                    open();
  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::stream>
                    open(QString const& id);
  unsigned int      opened_streams() const;
  void              set_initial_count(unsigned int cnt);
  void              set_initial_replay_events(bool replay);
  void              set_initial_store_events(bool store);
  void              set_process(bool in, bool out);
  void              set_succeed(bool succeed);
  QList<com::centreon::broker::misc::shared_ptr<setable_stream> >&
                    streams();

private:
  void              _internal_copy(setable_endpoint const& se);

  unsigned int      _initial_count;
  bool              _initial_replay_events;
  bool              _initial_store_events;
  unsigned int      _opened_streams;
  bool              _should_succeed;
  QList<com::centreon::broker::misc::shared_ptr<setable_stream> >
                    _streams;
};

#endif // !CCB_SETABLE_ENDPOINT_HH
