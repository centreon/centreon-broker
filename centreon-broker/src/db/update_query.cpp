/*
** update_query.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/03/09 Matthieu Kermagoret
*/

#include "db/update_query.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the UpdateQuery object to the current instance.
 */
void UpdateQuery::InternalCopy(const UpdateQuery& update_query)
{
  this->uniques_ = update_query.uniques_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  UpdateQuery default constructor.
 */
UpdateQuery::UpdateQuery()
{
}

/**
 *  UpdateQuery copy constructor.
 */
UpdateQuery::UpdateQuery(const UpdateQuery& update_query) : Query(update_query)
{
  this->InternalCopy(update_query);
}

/**
 *  UpdateQuery destructor.
 */
UpdateQuery::~UpdateQuery()
{
}

/**
 *  UpdateQuery operator= overload.
 */
UpdateQuery& UpdateQuery::operator=(const UpdateQuery& update_query)
{
  this->Query::operator=(update_query);
  this->InternalCopy(update_query);
  return (*this);
}

/**
 *  Add some unique fields that'll help when updating an object.
 */
void UpdateQuery::AddUniques(const char** uniques)
{
  for (unsigned int i = 0; uniques[i]; i++)
    this->uniques_.push_back(uniques[i]);
  return ;
}
