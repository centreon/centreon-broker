##
## Makefile for Centreon-Broker in .
## 
## Made by Matthieu Kermagoret <mkermagoret@merethis.com>
## 
## Copyright Merethis
## See LICENSE file for details.
## 
## Started on  05/04/09 Matthieu Kermagoret
## Last update 05/13/09 Matthieu Kermagoret
##

CXX		=	g++
RM		=	rm -rf

SRC		=	src/cb2db.cpp			\
			src/conditionvariable.cpp	\
			src/event.cpp			\
			src/eventpublisher.cpp		\
			src/eventsubscriber.cpp		\
			src/exception.cpp		\
			src/fileoutput.cpp		\
			src/hoststatusevent.cpp		\
			src/mutex.cpp			\
			src/mysqloutput.cpp		\
			src/networkinput.cpp		\
			src/servicestatusevent.cpp	\
			src/thread.cpp
OBJ		=	$(SRC:.cpp=.o)

NAME		=	cb2db

INCLUDE		+=	-Iinc
CXXFLAGS	+=	-g3 --std=c++0x -W -Wall -pedantic $(INCLUDE)	\
			 `mysql_config --include`
LDFLAGS		+=	-lpthread -lrt `mysql_config --libs`


all		:	$(NAME)

$(NAME)		:	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean		:
	$(RM) `find . -name '*~' -or -name '#*#'`  $(OBJ)

fclean		:	clean
	$(RM) $(NAME)

re		:	fclean all

.PHONY		:	all clean fclean re
