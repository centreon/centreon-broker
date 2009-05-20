/*
** instance.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/20/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#include <string>
#include "instance.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of the given object to the current
 *  instance.
 */
void Instance::InternalCopy(const Instance& inst)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = inst.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Instance default constructor.
 */
Instance::Instance()
{
}

/**
 *  Instance copy constructor.
 */
Instance::Instance(const Instance& inst) : Event(inst)
{
  this->InternalCopy(inst);
}

/**
 *  Instance destructor.
 */
Instance::~Instance()
{
}

/**
 *  Instance operator= overload.
 */
Instance& Instance::operator=(const Instance& inst)
{
  this->Event::operator=(inst);
  this->InternalCopy(inst);
  return (*this);
}

/**
 *  Returns the type of this Event.
 */
int Instance::GetType() const throw ()
{
  // XXX : correct with NDO_API_SOMETHING
  return (3);
}
