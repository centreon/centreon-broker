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
#include "io/net/ipv4.h"
#include "processing/listener.h"
#include "processing/manager.h"

int main()
{
  std::auto_ptr<IO::Net::IPv4Acceptor> ipv4(new IO::Net::IPv4Acceptor);
  std::auto_ptr<Processing::Listener> listener(new Processing::Listener);

  ipv4->Listen(5667);
  listener->Init(ipv4.get(), Processing::Listener::NDO);
  ipv4.release();
  Processing::Manager::Instance().Manage(listener.get());

  sleep(30);

  //Processing::Manager::Instance().Delete(listener.release());
  return (0);
}
