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

# Utilities
CXX	=	g++
RM	=	rm -rf

# Common source files.
COMSRC	=	../../src/concurrency/condition_variable.cpp		\
		../../src/concurrency/lock.cpp				\
		../../src/concurrency/mutex.cpp				\
		../../src/concurrency/thread.cpp			\
		../../src/concurrency/thread_listener.cpp		\
		../../src/configuration/interface.cpp			\
		../../src/configuration/lexer.cpp			\
		../../src/configuration/log.cpp				\
		../../src/configuration/manager.cpp			\
		../../src/configuration/parser.cpp			\
		../../src/db/connection.cpp				\
		../../src/db/db_exception.cpp				\
		../../src/db/delete.cpp					\
		../../src/db/have_args.cpp				\
		../../src/db/have_fields.cpp				\
		../../src/db/have_predicate.cpp				\
		../../src/db/have_table.cpp				\
		../../src/db/insert.cpp					\
		../../src/db/mysql/connection.cpp			\
		../../src/db/mysql/delete.cpp				\
		../../src/db/mysql/have_args.cpp			\
		../../src/db/mysql/have_predicate.cpp			\
		../../src/db/mysql/insert.cpp				\
		../../src/db/mysql/query.cpp				\
		../../src/db/mysql/select.cpp				\
		../../src/db/mysql/truncate.cpp				\
		../../src/db/mysql/update.cpp				\
		../../src/db/predicate.cpp				\
		../../src/db/query.cpp					\
		../../src/db/select.cpp					\
		../../src/db/truncate.cpp				\
		../../src/db/update.cpp					\
		../../src/events/acknowledgement.cpp			\
		../../src/events/comment.cpp				\
		../../src/events/dependency.cpp				\
		../../src/events/downtime.cpp				\
		../../src/events/event.cpp				\
		../../src/events/group.cpp				\
		../../src/events/group_member.cpp			\
		../../src/events/host.cpp				\
		../../src/events/host_dependency.cpp			\
		../../src/events/host_group.cpp				\
		../../src/events/host_group_member.cpp			\
		../../src/events/host_parent.cpp			\
		../../src/events/host_service.cpp			\
		../../src/events/host_service_status.cpp		\
		../../src/events/host_status.cpp			\
		../../src/events/log.cpp				\
		../../src/events/program_status.cpp			\
		../../src/events/service.cpp				\
		../../src/events/service_dependency.cpp			\
		../../src/events/service_group.cpp			\
		../../src/events/service_group_member.cpp		\
		../../src/events/service_status.cpp			\
		../../src/events/status.cpp				\
		../../src/exception.cpp					\
		../../src/init.cpp					\
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
		../../src/io/tls/acceptor.cpp				\
		../../src/io/tls/connector.cpp				\
		../../src/io/tls/internal.cpp				\
		../../src/io/tls/params.cpp				\
		../../src/io/tls/stream.cpp				\
		../../src/logging.cpp					\
		../../src/mapping.cpp					\
		../../src/multiplexing/internal.cpp			\
		../../src/multiplexing/publisher.cpp			\
		../../src/multiplexing/subscriber.cpp			\
		../../src/processing/failover_out.cpp			\
		../../src/processing/feeder.cpp				\
		../../src/processing/listener.cpp			\
		../../src/processing/listener_destination.cpp
COMOBJ	=	$(COMSRC:.cpp=.o)

# cb2db
SRC1	=	../../src/daemon/main.cpp
OBJ1	=	$(SRC1:.cpp=.o)
NAME1	=	cb2db

# cbmod
SRC2	=	../../src/module/callbacks.cpp				\
		../../src/module/initial.cpp				\
		../../src/module/main.cpp
OBJ2	=	$(SRC2:.cpp=.o)
NAME2	=	cbmod.so

# Parameters
INCLUDE		=	-I../../inc/daemon -I../../inc/module -I../../inc
MACROS		=	-DUSE_MYSQL -DUSE_TLS -DTIXML_USE_STL
# GCC CXXFLAGS
#CXXFLAGS	=	-O3 -pedantic -Wall -Wextra -DNDEBUG
CXXFLAGS	=	-g3 -O0 -pedantic -Wall -Wextra
# ICC CXXFLAGS
#CXXFLAGS	=	-w1 -Wall -Wcheck
CXXFLAGS	+=	$(INCLUDE) $(MACROS) `mysql_config --include`
LDFLAGS		=	-lpthread `mysql_config --libs_r` -lgnutls #-lefence

# Rules
all		:	$(NAME1) $(NAME2)

$(NAME1)	:	$(COMOBJ) $(OBJ1)
	$(CXX) -o $(NAME1) $(COMOBJ) $(OBJ1) $(LDFLAGS)

$(NAME2)	:	$(COMOBJ) $(OBJ2)
	$(CXX) -shared -o $(NAME2) $(COMOBJ) $(OBJ2) $(LDFLAGS)

clean		:
	$(RM) $(COMOBJ) $(OBJ1) $(OBJ2) `find . -name '*~' -or -name '#*#'`

fclean		:	clean
	$(RM) $(NAME1) $(NAME2)

re		:	fclean all

.PHONY		:	all clean fclean re
