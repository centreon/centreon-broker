/*
** truncate.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/09/09 Matthieu Kermagoret
*/

#include "db/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Truncate copy constructor.
 */
Truncate::Truncate(const Truncate& truncate) throw () : Query()
{
  (void)truncate;
}

/**
 *  Truncate operator= overload.
 */
Truncate& Truncate::operator=(const Truncate& truncate) throw ()
{
  (void)truncate;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Truncate default constructor.
 */
Truncate::Truncate() throw ()
{
}

/**
 *  Truncate destructor.
 */
Truncate::~Truncate()
{
}
