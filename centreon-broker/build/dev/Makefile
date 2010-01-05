# Utilities
CXX	=	g++
RM	=	rm -rf

# Targets
SRC	=	../../src/concurrency/condition_variable.cpp		\
		../../src/concurrency/lock.cpp				\
		../../src/concurrency/mutex.cpp				\
		../../src/concurrency/thread.cpp			\
		../../src/concurrency/thread_listener.cpp		\
		../../src/daemon/configuration/interface.cpp		\
		../../src/daemon/configuration/lexer.cpp		\
		../../src/daemon/configuration/log.cpp			\
		../../src/daemon/configuration/manager.cpp		\
		../../src/daemon/interface/db/destination.cpp		\
		../../src/daemon/interface/destination.cpp		\
		../../src/daemon/interface/factory.cpp			\
		../../src/daemon/interface/ndo/destination.cpp		\
		../../src/daemon/interface/ndo/internal.cpp		\
		../../src/daemon/interface/ndo/source.cpp		\
		../../src/daemon/interface/source.cpp			\
		../../src/daemon/interface/sourcedestination.cpp	\
		../../src/daemon/interface/xml/destination.cpp		\
		../../src/daemon/interface/xml/internal.cpp		\
		../../src/daemon/interface/xml/tinystr.cpp		\
		../../src/daemon/interface/xml/tinyxml.cpp		\
		../../src/daemon/interface/xml/tinyxmlerror.cpp		\
		../../src/daemon/interface/xml/tinyxmlparser.cpp	\
		../../src/daemon/main.cpp				\
		../../src/daemon/multiplexing/publisher.cpp		\
		../../src/daemon/multiplexing/subscriber.cpp		\
		../../src/daemon/processing/delivery.cpp		\
		../../src/daemon/processing/feeder.cpp			\
		../../src/daemon/processing/high_availability.cpp	\
		../../src/daemon/processing/listener.cpp		\
		../../src/daemon/processing/listener_destination.cpp	\
		../../src/daemon/processing/manager.cpp			\
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
		../../src/events/downtime.cpp				\
		../../src/events/event.cpp				\
		../../src/events/group.cpp				\
		../../src/events/host.cpp				\
		../../src/events/host_group.cpp				\
		../../src/events/host_service.cpp			\
		../../src/events/host_service_status.cpp		\
		../../src/events/host_status.cpp			\
		../../src/events/log.cpp				\
		../../src/events/program_status.cpp			\
		../../src/events/service.cpp				\
		../../src/events/service_group.cpp			\
		../../src/events/service_status.cpp			\
		../../src/events/status.cpp				\
		../../src/exception.cpp					\
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
		../../src/mapping.cpp
OBJ	=	$(SRC:.cpp=.o)
NAME	=	cb2db

# Parameters
INCLUDE		=	-I../../inc/daemon -I../../inc
MACROS		=	-DUSE_MYSQL -DUSE_TLS -DTIXML_USE_STL
# GCC CXXFLAGS
CXXFLAGS	=	-O3 -pedantic -Wall -Wextra -DNDEBUG
#CXXFLAGS	=	-g3 -O0 -pedantic -Wall -Wextra
# ICC CXXFLAGS
#CXXFLAGS	=	-w1 -Wall -Wcheck
CXXFLAGS	+=	$(INCLUDE) $(MACROS) `mysql_config --include`
LDFLAGS		=	-lpthread `mysql_config --libs_r` -lboost_thread-mt -lgnutls #-lefence

# Rules
all	:	$(NAME)

$(NAME)	:	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean	:
	$(RM) $(OBJ) `find . -name '*~' -or -name '#*#'`

fclean	:	clean
	$(RM) $(NAME)

re	:	fclean all

.PHONY	:	all clean fclean re
