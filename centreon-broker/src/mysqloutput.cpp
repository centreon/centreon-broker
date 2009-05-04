/*
** mysqloutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#include "mysqloutput.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

MySQLOutput::MySQLOutput(const MySQLOutput& mysqlo) : Output(), Thread()
{
  (void)mysqlo;
}

MySQLOutput&		MySQLOutput::operator=(const MySQLOutput& mysqlo)
{
  (void)mysqlo;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

MySQLOutput::MySQLOutput() : Output(), Thread()
{
}

MySQLOutput::~MySQLOutput()
{
}

void			MySQLOutput::Event(const CentreonBroker::Event& event)
{
}

int			MySQLOutput::Core()
{
}
