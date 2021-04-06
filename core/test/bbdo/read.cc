///*
//** Copyright 2015 Centreon
//**
//** Licensed under the Apache License, Version 2.0 (the "License");
//** you may not use this file except in compliance with the License.
//** You may obtain a copy of the License at
//**
//**     http://www.apache.org/licenses/LICENSE-2.0
//**
//** Unless required by applicable law or agreed to in writing, software
//** distributed under the License is distributed on an "AS IS" BASIS,
//** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//** See the License for the specific language governing permissions and
//** limitations under the License.
//**
//** For more information : contact@centreon.com
//*/
//
//#include <arpa/inet.h>
//#include <gtest/gtest.h>
//#include <bitset>
//#include <iostream>
//#include "com/centreon/broker/bbdo/internal.hh"
//#include "com/centreon/broker/bbdo/stream.hh"
//#include "com/centreon/broker/config/applier/init.hh"
//#include "com/centreon/broker/exceptions/msg.hh"
//#include "com/centreon/broker/extcmd/command_request.hh"
//#include "com/centreon/broker/extcmd/internal.hh"
//#include "com/centreon/broker/file/stream.hh"
//#include "com/centreon/broker/io/events.hh"
//#include "com/centreon/broker/io/raw.hh"
//#include "com/centreon/broker/misc/misc.hh"
//#include "com/centreon/broker/neb/service.hh"
//#include "com/centreon/broker/modules/loader.hh"
//
// using namespace com::centreon::broker;
//
//// Valid empty event (a command_request).
//#define EMPTY_EVENT "000E000800020000000000000000000000000000"
//// This event does not have enough data for its payload.
//#define NOT_ENOUGH_DATA_EVENT "0009000800020000000000000000000000000000"
//// This event is of invalid type.
//#define INVALID_EVENT_TYPE "000000420042"
//
// class from_memory : public io::stream {
// public:
//  from_memory() : sent_data(false), _memory() {}
//  from_memory(std::vector<char> const& memory)
//      : sent_data{false}, _memory{memory} {}
//  ~from_memory() {}
//
//  bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1) {
//    (void)deadline;
//    std::cout << "from memory read...\n";
//    if (sent_data)
//      throw exceptions::msg() << "shutdown";
//    std::shared_ptr<io::raw> raw(new io::raw);
//    raw->get_buffer() = _memory;
//    d = raw;
//    sent_data = true;
//    return true;
//  }
//
//  int write(std::shared_ptr<io::data> const& d) {
//    _memory = std::static_pointer_cast<io::raw>(d)->get_buffer();
//    return 1;
//  }
//
//  std::vector<char> const& get_memory() const { return _memory; }
//
// private:
//  bool sent_data;
//  std::vector<char> _memory;
//};
//
// class ReadTest : public ::testing::Test {
// public:
//  void SetUp() {
//    // Initialization
//    try {
//      config::applier::init();
//    } catch (std::exception const& e) {
//      (void)e;
//    }
//    extcmd::load();
//  }
//
//  void TearDown() {
//    // Cleanup.
//    config::applier::deinit();
//  }
//};
//
///**
// *  Compute and prepend the checksum to a packet.
// *
// *  @param packet  The packet.
// */
// static void prepend_checksum(std::vector<char>& packet) {
//  uint16_t checksum{htons(misc::crc16_ccitt(&packet[0], 6))};
//  packet.insert(packet.begin(), (char*)&checksum,
//                ((char*)&checksum) + sizeof(uint16_t));
//}
//
///**
// *  Append an empty event at the end of the packet.
// *  @param[in] packet  The empty raw event.
// */
// static void append_empty_valid_event(std::vector<char>& packet) {
//  std::vector<char> packet2{misc::from_hex(EMPTY_EVENT)};
//  uint16_t checksum{htons(misc::crc16_ccitt(&packet2[0], 14))};
//  packet2.insert(packet2.begin(), (char*)&checksum,
//                ((char*)&checksum) + sizeof(uint16_t));
//  packet.insert(packet.end(), packet2.begin(), packet2.end());
//}
//
// TEST_F(ReadTest, EmptyEvent) {
//  modules::loader l;
//  l.load_file("./neb/10-neb.so");
//
//  std::shared_ptr<neb::service> svc{nullptr};
//
//  std::shared_ptr<io::stream> stream;
//  std::shared_ptr<from_memory> memory_stream(new from_memory());
//  bbdo::stream stm;
//  stm.set_substream(memory_stream);
//  stm.set_coarse(false);
//  stm.set_negotiate(false);
//  stm.negotiate(bbdo::stream::negotiate_first);
//  stm.write(svc);
//  std::vector<char> const& mem1 = memory_stream->get_memory();
//  for (uint32_t i = 0; i < mem1.size(); i++) {
//    std::cout << std::hex << (static_cast<uint32_t>(mem1[i]) & 0xff) << ' ';
//  }
//  std::cout << '\n';
//
//  std::shared_ptr<io::data> d;
//  ASSERT_NO_THROW(stm.read(d));
//  ASSERT_TRUE(!d);
//
////  ASSERT_EQ(mem1.size(), 276);
////  // The size is 276 - 16: 16 is the header size.
////  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 146)),
////            0x11223344);
////  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 150)),
////            0x55667788);
////  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 2)), 260);
////  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 91)),
/// 12345u); /  ASSERT_EQ(std::string(&mem1[0] + 265), "Bonjour");
////
////  ASSERT_EQ(std::string(&mem1[0] + 265), "Bonjour");
////  svc->host_id = 78113;
////  svc->service_id = 18;
////  svc->output = "Conjour";
////  stm.write(svc);
////  std::vector<char> const& mem2 = memory_stream->get_memory();
////
////  ASSERT_EQ(mem2.size(), 276);
////  ASSERT_EQ(htonl(*reinterpret_cast<uint32_t const*>(&mem1[0] + 91)),
/// 78113u); /  // The size is 276 - 16: 16 is the header size. /
/// ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0] + 2)), 260);
////
////  // Check checksum
////  ASSERT_EQ(htons(*reinterpret_cast<uint16_t const*>(&mem1[0])), 33491);
////
////  ASSERT_EQ(std::string(&mem1[0] + 265), "Conjour");
//  l.unload();
//}
///**
// *  Test a packet containing no event.
// */
// TEST_F(ReadTest, EmptyEvent1) {
//  std::vector<char> packet{misc::from_hex(EMPTY_EVENT)};
//  uint16_t checksum{htons(misc::crc16_ccitt(&packet[0], 14))};
//  packet.insert(packet.begin(), (char*)&checksum,
//                ((char*)&checksum) + sizeof(uint16_t));
//
//  std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
//  bbdo::stream stream;
//  stream.set_substream(memory_stream);
//  std::shared_ptr<io::data> d;
//  ASSERT_THROW(stream.read(d), std::exception);
//  ASSERT_TRUE(d);
//}
//
///**
// *  Test a packet containing not enough data.
// */
// TEST_F(ReadTest, NotEnoughData) {
//  std::vector<char> packet{misc::from_hex(NOT_ENOUGH_DATA_EVENT)};
//  prepend_checksum(packet);
//  std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
//  bbdo::stream stream;
//  stream.set_substream(memory_stream);
//  std::shared_ptr<io::data> d;
//  ASSERT_THROW(stream.read(d), std::exception);
//}
//
///**
// *  Test an event of invalid type.
// */
// TEST_F(ReadTest, InvalidEvent) {
//  std::vector<char> packet{misc::from_hex(INVALID_EVENT_TYPE)};
//  prepend_checksum(packet);
//  append_empty_valid_event(packet);
//  std::shared_ptr<from_memory> memory_stream =
//  std::make_shared<from_memory>(packet); bbdo::stream stream;
//  stream.set_coarse(true);
//  stream.set_substream(memory_stream);
//  std::shared_ptr<io::data> d;
//  // The first is null.
//  ASSERT_THROW(stream.read(d), std::exception);
//  ASSERT_TRUE(!d);
//
//  std::cout << "This event should be good\n";
//  // The second is an extcmd.
//  stream.read(d);
//  std::cout << "This event should also be good\n";
//  ASSERT_TRUE(d && d->type() == extcmd::command_request::static_type());
//}
//
///**
// *  Test an event of with bad checksum.
// */
// TEST_F(ReadTest, InvalidChecksum) {
//  std::vector<char> packet{misc::from_hex(EMPTY_EVENT)};
//  // Bogus checksum.
//  char head[] = {42, 42};
//  packet.insert(packet.begin(), head, head + 2);
//  append_empty_valid_event(packet);
//  std::shared_ptr<from_memory> memory_stream(new from_memory(packet));
//  bbdo::stream stream;
//  stream.set_coarse(true);
//  stream.set_substream(memory_stream);
//  std::shared_ptr<io::data> d;
//  stream.read(d);
//  ASSERT_TRUE(d &&d->type() == extcmd::command_request::static_type());
//}
