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

CXX		=	g++
RM		=	rm -rf

SRC		=	src/acknowledgement.cpp			\
			src/cb2db.cpp				\
			src/comment.cpp				\
			src/conf/conf.cpp			\
			src/conf/input.cpp			\
			src/conf/log.cpp			\
			src/conf/output.cpp			\
			src/connection.cpp			\
			src/connection_status.cpp		\
			src/db/connection.cpp			\
			src/db/db_exception.cpp			\
			src/db/have_fields.cpp			\
			src/db/mysql/connection.cpp		\
			src/db/mysql/have_fields.cpp		\
			src/db/mysql/query.cpp			\
			src/db/mysql/truncate.cpp		\
			src/db/postgresql/connection.cpp	\
			src/db/predicate.cpp			\
			src/db/query.cpp			\
			src/db/truncate.cpp			\
			src/db_output.cpp			\
			src/event.cpp				\
			src/event_publisher.cpp			\
			src/event_subscriber.cpp		\
			src/exception.cpp			\
			src/host.cpp                            \
			src/host_service.cpp			\
			src/host_service_status.cpp		\
			src/host_status.cpp			\
			src/instance.cpp			\
			src/logging.cpp				\
			src/mapping.cpp				\
			src/network_acceptor.cpp		\
			src/network_input.cpp			\
			src/program_status.cpp			\
			src/protocol_socket.cpp			\
			src/service.cpp				\
			src/service_status.cpp			\
			src/status.cpp
OBJ		=	$(SRC:.cpp=.o)

NAME		=	cb2db

INCLUDE		+=	-Iinc
CXXFLAGS	+=	-std=c++0x -W -Wall -pedantic $(INCLUDE)	\
			`mysql_config --include`			\
			-I`pg_config --includedir`
## Debug
#CXXFLAGS	+=	-g3
## Release
CXXFLAGS	+=	-O2 -DNDEBUG
LDFLAGS		+=	-lpthread -lrt `mysql_config --libs_r`		\
			-lboost_system-mt -lboost_thread-mt		\
			-L`pg_config --libdir` -lpq
## Features
CXXFLAGS	+=	-DUSE_MYSQL -DUSE_POSTGRESQL -DUSE_TLS

all		:	$(NAME)

$(NAME)		:	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean		:
	$(RM) `find . -name '*~' -or -name '#*#'`  $(OBJ)

fclean		:	clean
	$(RM) $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re
