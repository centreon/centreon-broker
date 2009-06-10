/*
** have_fields.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#include <cassert>
#include "db/have_fields.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  HaveFields copy constructor. No reason we would want to copy this
 *  class so declare it private.
 */
HaveFields::HaveFields(const HaveFields& hf) throw ()
{
  (void)hf;
  assert(false);
}

/**
 *  HaveFields operator= overload. No reason we would want to copy this
 *  class so declare it private.
 */
HaveFields& HaveFields::operator=(const HaveFields& hf) throw ()
{
  (void)hf;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HaveFields constructor.
 */
HaveFields::HaveFields() throw ()
{
}

/**
 *  HaveFields destructor.
 */
HaveFields::~HaveFields()
{
}

