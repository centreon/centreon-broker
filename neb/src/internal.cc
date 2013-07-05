/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

// Configuration file name.
QString neb::gl_configuration_file;

// Instance information.
unsigned int neb::instance_id;
QString      neb::instance_name;

// List of host IDs.
umap<std::string, int> neb::gl_hosts;

// List of service IDs.
std::map<std::pair<std::string, std::string>, std::pair<int, int> > neb::gl_services;

// Sender object.
multiplexing::publisher neb::gl_publisher;
