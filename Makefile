##
## Makefile for Centreon-Broker in .
## 
## Made by Matthieu Kermagoret <mkermagoret@merethis.com>
## 
## Copyright Merethis
## See LICENSE file for details.
## 
## Started on  05/04/09 Matthieu Kermagoret
## Last update 06/15/09 Matthieu Kermagoret
##

CXX		=	g++
RM		=	rm -rf

SRC		=	src/cb2db.cpp				\
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
LDFLAGS		+=	-lpthread -lrt `mysql_config --libs`		\
			-lboost_system-mt -lboost_thread-mt		\
			-L`pg_config --libdir` -lpq


all		:	$(NAME)

$(NAME)		:	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean		:
	$(RM) `find . -name '*~' -or -name '#*#'`  $(OBJ)

fclean		:	clean
	$(RM) $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re
