/*
** Copyright 2013 Centreon
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

#include <arpa/inet.h>
#include <stdint.h>
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/cbd.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  Build a BBDO packet.
 *
 */
static void build_bbdo_version_packet(std::string& packet,
                                      unsigned short major,
                                      unsigned short minor,
                                      unsigned short patch,
                                      std::string extensions = "") {
  // Checksum (set later).
  packet.resize(2);

  // Packet size.
  {
    uint16_t size(htons(7 + extensions.size()));
    packet.append(static_cast<char*>(static_cast<void*>(&size)), sizeof(size));
  }

  // Type.
  {
    uint32_t type(htonl(65535u << 16 | 1));
    packet.append(static_cast<char*>(static_cast<void*>(&type)), sizeof(type));
  }

  // Serialization.
  major = htons(major);
  packet.append(static_cast<char*>(static_cast<void*>(&major)), sizeof(major));
  minor = htons(minor);
  packet.append(static_cast<char*>(static_cast<void*>(&minor)), sizeof(minor));
  patch = htons(patch);
  packet.append(static_cast<char*>(static_cast<void*>(&patch)), sizeof(patch));
  packet.append(extensions);
  packet.append("", 1);

  // Checksum.
  {
    uint16_t chksum(qChecksum(packet.data() + 2, 6));
    packet[0] = chksum >> 8;
    packet[1] = chksum & 0xFF;
  }

  return;
}

/**
 *  Check that BBDO properly check protocol version.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Error flag.
  bool error(true);

  // Qt object.
  QCoreApplication app(argc, argv);

  // Variables that need cleaning.
  cbd broker;

  try {
    // Prepare server connection.
    QTcpServer servr;
    servr.listen(QHostAddress::Any, 5685);

    // Launch daemon.
    broker.set_config_file(PROJECT_SOURCE_DIR
                           "/test/cfg/bbdo_protocol_version.xml");
    broker.start();
    sleep_for(2);

    // Server connection.
    {
      // Connection.
      if (!servr.waitForNewConnection(-1))
        throw(exceptions::msg() << "cannot wait for new incoming connection");
      std::auto_ptr<QTcpSocket> sockt(servr.nextPendingConnection());
      if (!sockt.get())
        throw(exceptions::msg() << "no incoming connection");

      // Wait for peer version packet.
      sockt->waitForReadyRead(-1);
      do {
        sockt->readAll();
      } while (sockt->waitForReadyRead(0));

      // Write version packet.
      std::string packet;
      build_bbdo_version_packet(packet, 9, 9, 9, "");
      char const* ptr(packet.data());
      unsigned int size(packet.size());
      while (size != 0) {
        qint64 wb(sockt->write(ptr, size));
        if (wb == -1)
          throw(exceptions::msg()
                << "cannot write BBDO packet to incoming peer: "
                << sockt->errorString());
        ptr += wb;
        size -= wb;
      }
      sockt->waitForBytesWritten(-1);

      // Peer should close connection.
      sleep_for(2);
      if (sockt->waitForReadyRead(2 * MONITORING_ENGINE_INTERVAL_LENGTH *
                                  1000) ||
          ((sockt->state() != QAbstractSocket::ClosingState) &&
           (sockt->state() != QAbstractSocket::UnconnectedState)))
        throw(exceptions::msg() << "incoming peer did not close "
                                << "connection despite we provided an invalid "
                                << "protocol version");
    }

    // Client connection.
    {
      // Connection.
      QTcpSocket sockt;
      sockt.connectToHost("localhost", 5686);
      if (!sockt.waitForConnected(-1))
        throw(exceptions::msg() << "cannot connect to cbd");

      // Write version packet.
      std::string packet;
      build_bbdo_version_packet(packet, 9, 9, 9, "");
      char const* ptr(packet.data());
      unsigned int size(packet.size());
      while (size != 0) {
        qint64 wb(sockt.write(ptr, size));
        if (wb == -1)
          throw(exceptions::msg()
                << "cannot write BBDO packet to connected peer");
        ptr += wb;
        size -= wb;
      }
      sockt.waitForBytesWritten(-1);

      // Peer should close connection.
      sleep_for(2);
      if (sockt.waitForReadyRead(2 * MONITORING_ENGINE_INTERVAL_LENGTH *
                                 1000) ||
          ((sockt.state() != QAbstractSocket::ClosingState) &&
           (sockt.state() != QAbstractSocket::UnconnectedState)))
        throw(exceptions::msg() << "connected peer did not close "
                                << "connection despite we provided an invalid "
                                << "protocol version");
    }

    // Success.
    error = false;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }

  // Cleanup.
  broker.stop();
  QTimer::singleShot(100, &app, SLOT(quit()));
  app.exec();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
