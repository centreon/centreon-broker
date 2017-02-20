/*
** Copyright 2017 Centreon
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

#include <gtest/gtest.h>
#include <memory>
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"

using namespace com::centreon::broker;

class                  MultiplexingMuxerRead : public ::testing::Test {
 public:
  void                 setup(std::string const& name) {
    _m.reset(new multiplexing::muxer(name, false));
    multiplexing::muxer::filters f;
    f.insert(io::raw::static_type());
    _m->set_read_filters(f);
    _m->set_write_filters(f);
    return ;
  }

  void                 publish_events(int count = 10000) {
    for (int i(0); i < count; ++i) {
      misc::shared_ptr<io::raw> r(new io::raw());
      r->resize(sizeof(i));
      memcpy(r->QByteArray::data(), &i, sizeof(i));
      _m->publish(r);
    }
    return ;
  }

  void                 reread_events(int from = 0, int to = 10000) {
    misc::shared_ptr<io::data> d;
    for (int i(from); i < to; ++i) {
      d.clear();
      _m->read(d, 0);
      ASSERT_FALSE(d.isNull());
      ASSERT_EQ(d->type(), io::raw::static_type());
      int reread;
      memcpy(
        &reread,
        d.staticCast<io::raw>()->QByteArray::data(),
        sizeof(reread));
      ASSERT_EQ(reread, i);
    }
    return ;
  }

 protected:
  std::auto_ptr<multiplexing::muxer> _m;
};

// Given a muxer object with all filters
// When some events are given to publish()
// Then I can read() the events back
TEST_F(MultiplexingMuxerRead, Read) {
  setup("MultiplexingMuxerRead_Read");
  publish_events();
  reread_events();
  misc::shared_ptr<io::data> d;
  _m->read(d, 0);
  ASSERT_TRUE(d.isNull());
}

// Given a muxer object with all filters
// And some events were given to write()
// And the events were read() back
// When I call nack_events()
// Then I can read() the events back
TEST_F(MultiplexingMuxerRead, NackEvents) {
  setup("MultiplexingMuxerRead_NackEvents");
  publish_events();
  reread_events();
  misc::shared_ptr<io::data> d;
  _m->read(d, 0);
  ASSERT_TRUE(d.isNull());
  _m->nack_events();
  reread_events();
  _m->read(d, 0);
  ASSERT_TRUE(d.isNull());
}

// Given a muxer object with all filters
// And some events were given to write()
// And the events were read() back
// When all events are acknowledged with ack_events()
// Then I can read none of the events back
TEST_F(MultiplexingMuxerRead, AckEventsAll) {
  setup("MultiplexingMuxerRead_AckEventsAll");
  publish_events();
  reread_events();
  _m->ack_events(10000);
  misc::shared_ptr<io::data> d;
  _m->read(d, 0);
  ASSERT_TRUE(d.isNull());
}

// Given a muxer object with all filters
// And some events were given to write()
// And the events were read() back
// And events were partially acknowledged with ack_events()
// When I call nack_events()
// Then I can read() the unacknowledged events back
TEST_F(MultiplexingMuxerRead, AckEventsPartial) {
  setup("MultiplexingMuxerRead_AckEventsPartial");
  publish_events();
  reread_events();
  _m->ack_events(5000);
  _m->nack_events();
  reread_events(5000);
  misc::shared_ptr<io::data> d;
  _m->read(d, 0);
  ASSERT_TRUE(d.isNull());
}
