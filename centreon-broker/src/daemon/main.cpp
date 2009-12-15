/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <unistd.h>
#include "db/mysql/connection.h"
#include "interface/db/destination.h"
#include "interface/ndo/source.h"
#include "interface/xml/destination.h"
#include "io/net/ipv4.h"
#include "mapping.h"
#include "multiplexing/publisher.h"
#include "processing/high_availability.h"
#include "processing/listener.h"
#include "processing/listener_destination.h"
#include "processing/manager.h"

int main()
{
  std::auto_ptr<IO::Net::IPv4Acceptor> ipv4(new IO::Net::IPv4Acceptor);
  std::auto_ptr<Processing::Listener> listener(new Processing::Listener);
  CentreonBroker::DB::Connection* conn;
  Interface::DB::Destination* dest;
  Processing::HighAvailability* ha;

  MappingsInit();
  Interface::NDO::Source::Initialize();
  Interface::XML::Destination::Initialize();
  ipv4->Listen(5667);
  listener->Init(ipv4.get(),
                 Processing::Listener::NDO,
                 &Processing::Manager::Instance());
  ipv4.release();
  listener.release();

  conn = new CentreonBroker::DB::MySQLConnection();
  conn->Connect("localhost", "root", "123456789", "cb");
  dest = new Interface::DB::Destination();
  dest->Init(conn);
  /*ha = new Processing::HighAvailability();
    ha->Init(dest);*/
  std::auto_ptr<Processing::ListenerDestination> ld(new Processing::ListenerDestination);
  std::auto_ptr<IO::Net::IPv4Acceptor> acceptor(new IO::Net::IPv4Acceptor);

  acceptor->Listen(4242);
  ld->Init(acceptor.get(), Processing::ListenerDestination::XML);
  acceptor.release();

  while (1)
    pause();

  return (0);
}
