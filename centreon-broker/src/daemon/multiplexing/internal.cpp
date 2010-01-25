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

#include "multiplexing/internal.h"

/**************************************
*                                     *
*          Global Variables           *
*                                     *
**************************************/

// List of hosts.
std::map<std::pair<std::string, std::string>, int> Multiplexing::gl_hosts;
Concurrency::Mutex                                 Multiplexing::gl_hostsm;

// List of services.
std::map<std::pair<std::pair<std::string, std::string>, std::string>, int>
                                     Multiplexing::gl_services;
Concurrency::Mutex                   Multiplexing::gl_servicesm;

// List of subscribers.
std::list<Multiplexing::Subscriber*> Multiplexing::gl_subscribers;
Concurrency::Mutex                   Multiplexing::gl_subscribersm;
