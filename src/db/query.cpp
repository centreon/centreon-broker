/*
** query.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/02/09 Matthieu Kermagoret
** Last update 06/02/09 Matthieu Kermagoret
*/

#include "db/query.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the Query object to the current instance.
 */
void Query::InternalCopy(const Query& query)
{
  this->fields_ = query.fields_;
  this->table_ = query.table_;
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  Query default constructor.
 */
Query::Query()
{
}

/**
 *  Query copy constructor.
 */
Query::Query(const Query& query)
{
  this->InternalCopy(query);
}

/**
 *  Query destructor.
 */
Query::~Query()
{
}

/**
 *  Query operator= overload.
 */
Query& Query::operator=(const Query& query)
{
  this->InternalCopy(query);
  return (*this);
}

/**
 *  Add fields to the query.
 */
void Query::AddFields(const char** fields)
{
  for (unsigned int i = 0; fields[i]; i++)
    this->fields_.push_back(fields[i]);
  return ;
}

/**
 *  Set the table name.
 */
void Query::SetTable(const std::string& table)
{
  this->table_ = table;
  return ;
}
