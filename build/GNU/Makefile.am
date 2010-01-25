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

bin_PROGRAMS	=	cb2db cbmod.so


#######################################
#                                     #
#                cb2db                #
#                                     #
#######################################

cb2db_SOURCES=	../../src/concurrency/condition_variable.cpp		\
		../../src/concurrency/lock.cpp				\
		../../src/concurrency/mutex.cpp				\
		../../src/concurrency/thread.cpp			\
		../../src/concurrency/thread_listener.cpp		\
		../../src/daemon/configuration/interface.cpp		\
		../../src/daemon/configuration/lexer.cpp		\
		../../src/daemon/configuration/log.cpp			\
		../../src/daemon/configuration/manager.cpp		\
		../../src/daemon/configuration/parser.cpp		\
		../../src/daemon/main.cpp				\
		../../src/daemon/processing/failover_out.cpp		\
		../../src/daemon/processing/feeder.cpp			\
		../../src/daemon/processing/listener.cpp		\
		../../src/daemon/processing/listener_destination.cpp	\
		../../src/db/connection.cpp				\
		../../src/db/db_exception.cpp				\
		../../src/db/delete.cpp					\
		../../src/db/have_args.cpp				\
		../../src/db/have_fields.cpp				\
		../../src/db/have_predicate.cpp				\
		../../src/db/have_table.cpp				\
		../../src/db/insert.cpp					\
		../../src/db/predicate.cpp				\
		../../src/db/query.cpp					\
		../../src/db/select.cpp					\
		../../src/db/truncate.cpp				\
		../../src/db/update.cpp					\
		../../src/events/acknowledgement.cpp			\
		../../src/events/comment.cpp				\
		../../src/events/downtime.cpp				\
		../../src/events/event.cpp				\
		../../src/events/group.cpp				\
		../../src/events/group_member.cpp			\
		../../src/events/host.cpp				\
		../../src/events/host_group.cpp				\
		../../src/events/host_group_member.cpp			\
		../../src/events/host_service.cpp			\
		../../src/events/host_service_status.cpp		\
		../../src/events/host_status.cpp			\
		../../src/events/log.cpp				\
		../../src/events/program_status.cpp			\
		../../src/events/service.cpp				\
		../../src/events/service_group.cpp			\
		../../src/events/service_group_member.cpp		\
		../../src/events/service_status.cpp			\
		../../src/events/status.cpp				\
		../../src/exception.cpp					\
		../../src/interface/db/destination.cpp			\
		../../src/interface/destination.cpp			\
		../../src/interface/factory.cpp				\
		../../src/interface/ndo/base.cpp			\
		../../src/interface/ndo/destination.cpp			\
		../../src/interface/ndo/internal.cpp			\
		../../src/interface/ndo/log.cpp				\
		../../src/interface/ndo/source.cpp			\
		../../src/interface/ndo/sourcedestination.cpp		\
		../../src/interface/source.cpp				\
		../../src/interface/sourcedestination.cpp		\
		../../src/interface/xml/destination.cpp			\
		../../src/interface/xml/internal.cpp			\
		../../src/interface/xml/tinystr.cpp			\
		../../src/interface/xml/tinyxml.cpp			\
		../../src/interface/xml/tinyxmlerror.cpp		\
		../../src/interface/xml/tinyxmlparser.cpp		\
		../../src/io/acceptor.cpp				\
		../../src/io/fd.cpp					\
		../../src/io/file.cpp					\
		../../src/io/net/ipv4.cpp				\
		../../src/io/net/ipv6.cpp				\
		../../src/io/net/socket.cpp				\
		../../src/io/net/unix.cpp				\
		../../src/io/split.cpp					\
		../../src/io/stream.cpp					\
		../../src/io/text.cpp					\
		../../src/logging.cpp					\
		../../src/mapping.cpp					\
		../../src/multiplexing/internal.cpp			\
		../../src/multiplexing/publisher.cpp			\
		../../src/multiplexing/subscriber.cpp

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
cb2db_SOURCES +=	../../src/db/oracle/connection.cpp		\
			../../src/db/oracle/delete.cpp			\
			../../src/db/oracle/have_args.cpp		\
			../../src/db/oracle/have_predicate.cpp		\
			../../src/db/oracle/insert.cpp			\
			../../src/db/oracle/query.cpp			\
			../../src/db/oracle/select.cpp			\
			../../src/db/oracle/truncate.cpp		\
			../../src/db/oracle/update.cpp
endif

# Add PostgreSQL files if PostgreSQL is enabled
if POSTGRESQL
cb2db_SOURCES +=	../../src/db/postgresql/connection.cpp		\
			../../src/db/postgresql/delete.cpp		\
			../../src/db/postgresql/have_args.cpp		\
			../../src/db/postgresql/have_predicate.cpp	\
			../../src/db/postgresql/insert.cpp		\
			../../src/db/postgresql/query.cpp		\
			../../src/db/postgresql/select.cpp		\
			../../src/db/postgresql/truncate.cpp		\
			../../src/db/postgresql/update.cpp
endif

# Add TLS files is TLS is enabled
if TLS
cb2db_SOURCES +=	../../src/io/tls/acceptor.cpp			\
			../../src/io/tls/connector.cpp			\
			../../src/io/tls/internal.cpp			\
			../../src/io/tls/params.cpp			\
			../../src/io/tls/stream.cpp
endif

# Configure compilation flags properly
cb2db_CPPFLAGS	=	$(AM_CPPFLAGS) -I../../inc -I../../inc/daemon	\
			$(BOOST_CPPFLAGS) $(GNUTLS_CPPFLAGS)		\
			$(MYSQL_CPPFLAGS)
cb2db_CXXFLAGS	=	$(AM_CXXFLAGS) $(LIBGNUTLS_CFLAGS)		\
			$(ORACLE_OCI_CFLAGS) $(POSTGRESQL_CFLAGS)	\
			$(PTHREAD_CFLAGS)
cb2db_LDFLAGS	=	$(AM_LDFLAGS) $(BOOST_LDFLAGS)			\
			$(GNUTLS_LDFLAGS) $(MYSQL_LDFLAGS)		\
			$(ORACLE_OCI_LDFLAGS) $(POSTGRESQL_LDFLAGS)	\
			$(PTHREAD_LIBS)


#######################################
#                                     #
#                cbmod                #
#                                     #
#######################################

cbmod_so_SOURCES=	../../src/events/acknowledgement.cpp		\
			../../src/events/comment.cpp			\
			../../src/events/downtime.cpp			\
			../../src/events/event.cpp			\
			../../src/events/group.cpp			\
			../../src/events/group_member.cpp		\
			../../src/events/host.cpp			\
			../../src/events/host_group.cpp			\
			../../src/events/host_group_member.cpp		\
			../../src/events/host_service.cpp		\
			../../src/events/host_service_status.cpp	\
			../../src/events/host_status.cpp		\
			../../src/events/log.cpp			\
			../../src/events/program_status.cpp		\
			../../src/events/service.cpp			\
			../../src/events/service_group.cpp		\
			../../src/events/service_group_member.cpp	\
			../../src/events/service_status.cpp		\
			../../src/events/status.cpp			\
			../../src/exception.cpp				\
			../../src/module/callbacks.cpp			\
			../../src/module/main.cpp			\
			../../src/multiplexing/internal.cpp		\
			../../src/multiplexing/publisher.cpp		\
			../../src/multiplexing/subscriber.cpp

# Configure compilation flags properly
cbmod_so_CPPFLAGS=	$(AM_CPPFLAGS) -I../../inc -I../../inc/module	\
			$(BOOST_CPPFLAGS) $(GNUTLS_CPPFLAGS)
cbmod_so_CXXFLAGS=	$(AM_CXXFLAGS) -shared -fPIC
cbmod_so_LDFLAGS=	$(AM_LDFLAGS) $(BOOST_LDFLAGS) -shared -fPIC	\
			$(GNUTLS_LDFLAGS)
