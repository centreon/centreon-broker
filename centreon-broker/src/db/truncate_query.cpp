/*
** truncate_query.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/05/09 Matthieu Kermagoret
** Last update 06/05/09 Matthieu Kermagoret
*/

#include "db/truncate_query.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the TruncateQuery object to the current instance.
 */
void TruncateQuery::InternalCopy(const TruncateQuery& truncate_query)
{
  this->table_ = truncate_query.table_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  TruncateQuery default constructor.
 */
TruncateQuery::TruncateQuery()
{
}

/**
 *  TruncateQuery copy constructor.
 */
TruncateQuery::TruncateQuery(const TruncateQuery& truncate_query)
{
  this->InternalCopy(truncate_query);
}

/**
 *  TruncateQuery destructor.
 */
TruncateQuery::~TruncateQuery()
{
}

/**
 *  TruncateQuery operator= overload.
 */
TruncateQuery& TruncateQuery::operator=(const TruncateQuery& truncate_query)
{
  this->InternalCopy(truncate_query);
  return (*this);
}

/**
 *  Set the table that will be truncated.
 */
void TruncateQuery::SetTable(const std::string& table)
{
  this->table_ = table;
  return ;
}
