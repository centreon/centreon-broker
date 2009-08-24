##
##  Copyright 2009 MERETHIS
##  This file is part of CentreonBroker.
##
##  CentreonBroker is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by the Free
##  Software Foundation, either version 2 of the License, or (at your option)
##  any later version.
##
##  CentreonBroker is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
##  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
##  for more details.
##
##  You should have received a copy of the GNU General Public License along
##  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
##
##  For more information : contact@centreon.com
##

AUTOMAKE_OPTIONS=	subdir-objects

bin_PROGRAMS	=	cb2db
cb2db_SOURCES	=	../../src/cb2db.cpp				\
			../../src/client_acceptor.cpp			\
			../../src/conf/global.cpp			\
			../../src/conf/input.cpp			\
			../../src/conf/log.cpp				\
			../../src/conf/manager.cpp			\
			../../src/conf/output.cpp			\
			../../src/db/connection.cpp			\
			../../src/db/db_exception.cpp			\
			../../src/db/delete.cpp				\
			../../src/db/have_args.cpp			\
			../../src/db/have_fields.cpp			\
			../../src/db/have_predicate.cpp			\
			../../src/db/have_table.cpp			\
			../../src/db/insert.cpp				\
			../../src/db/predicate.cpp			\
			../../src/db/query.cpp				\
			../../src/db/select.cpp				\
			../../src/db/truncate.cpp			\
			../../src/db/update.cpp				\
			../../src/db_output.cpp				\
			../../src/event_publisher.cpp			\
			../../src/event_subscriber.cpp			\
			../../src/events/acknowledgement.cpp		\
			../../src/events/comment.cpp			\
			../../src/events/connection.cpp			\
			../../src/events/connection_status.cpp		\
			../../src/events/downtime.cpp			\
			../../src/events/event.cpp			\
			../../src/events/host.cpp			\
			../../src/events/host_group.cpp			\
			../../src/events/host_service.cpp		\
			../../src/events/host_service_status.cpp	\
			../../src/events/host_status.cpp		\
			../../src/events/log.cpp			\
			../../src/events/program_status.cpp		\
			../../src/events/service.cpp			\
			../../src/events/service_status.cpp		\
			../../src/events/status.cpp			\
			../../src/exception.cpp				\
			../../src/file_output.cpp			\
			../../src/io/fd.cpp				\
			../../src/io/io.cpp				\
			../../src/io/net4.cpp				\
			../../src/io/net6.cpp				\
			../../src/io/unix.cpp				\
			../../src/logging.cpp				\
			../../src/mapping.cpp				\
			../../src/network_input.cpp

# Add MySQL files if MySQL is enabled
if MYSQL
cb2db_SOURCES +=	../../src/db/mysql/connection.cpp		\
			../../src/db/mysql/delete.cpp			\
			../../src/db/mysql/have_args.cpp		\
			../../src/db/mysql/have_predicate.cpp		\
			../../src/db/mysql/insert.cpp			\
			../../src/db/mysql/query.cpp			\
			../../src/db/mysql/select.cpp			\
			../../src/db/mysql/truncate.cpp			\
			../../src/db/mysql/update.cpp
endif

# Add Oracle files if Oracle is enabled
if ORACLE
endif

# Add PostgreSQL files if PostgreSQL is enabled
if POSTGRESQL
cb2db_SOURCES +=	../../src/db/postgresql/connection.cpp		\
			../../src/db/postgresql/have_fields.cpp		\
			../../src/db/postgresql/query.cpp		\
			../../src/db/postgresql/truncate.cpp
endif

# Add TLS files is TLS is enabled
if TLS
cb2db_SOURCES +=	../../src/io/tls.cpp
endif
