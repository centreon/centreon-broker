/*
** logging.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/15/09 Matthieu Kermagoret
*/

#include <cassert>
#include <iostream>
#include <pthread.h>
#include "logging.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Global Variables          *
*                                     *
**************************************/

Logging CentreonBroker::logging;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Logging copy constructor.
 */
Logging::Logging(const Logging& l)
{
  (void)l;
  assert(false);
}

/**
 *  Logging operator= overload.
 */
Logging& Logging::operator=(const Logging& l)
{
  (void)l;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Logging default constructor.
 */
Logging::Logging()
{
}

/**
 *  Logging destructor.
 */
Logging::~Logging()
{
}

#ifndef NDEBUG
/**
 *  Add a debug output.
 */
void Logging::AddDebug(const char* str)
{
  int indent;
  std::map<pthread_t, int>::iterator it;
  boost::unique_lock<boost::mutex> lock(this->mutex_);
  pthread_t self;

  self = pthread_self();
  it = this->indent_.find(self);
  if (it == this->indent_.end())
    {
      this->indent_[self] = 0;
      indent = 0;
    }
  else
    indent = (*it).second;
  std::clog << '[' << (unsigned long)self << "] ";
  for (int i = 0; i < indent; i++)
    std::clog << "  ";
  std::clog << str << std::endl;
  return ;
}
#endif /* !NDEBUG */

/**
 *  Add an error output.
 */
void Logging::AddError(const char* str)
{
  int indent;
  std::map<pthread_t, int>::iterator it;
  boost::unique_lock<boost::mutex> lock(this->mutex_);
  pthread_t self;

  self = pthread_self();
  it = this->indent_.find(self);
  if (it == this->indent_.end())
    {
      this->indent_[self] = 0;
      indent = 0;
    }
  else
    indent = (*it).second;
  std::clog << '[' << (unsigned long)self << "] ";
  for (int i = 0; i < indent; i++)
    std::clog << "  ";
  std::clog << str << std::endl;
  return ;
}

/**
 *  Add an info output.
 */
void Logging::AddInfo(const char* str)
{
  int indent;
  std::map<pthread_t, int>::iterator it;
  boost::unique_lock<boost::mutex> lock(this->mutex_);
  pthread_t self;

  self = pthread_self();
  it = this->indent_.find(self);
  if (it == this->indent_.end())
    {
      this->indent_[self] = 0;
      indent = 0;
    }
  else
    indent = (*it).second;
  std::clog << '[' << (unsigned long)self << "] ";
  for (int i = 0; i < indent; i++)
    std::clog << "  ";
  std::clog << str << std::endl;
  return;
}

/**
 *  Remove a level of indentation to the current thread.
 */
void Logging::Deindent()
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  this->indent_[pthread_self()]--;
  return ;
}

/**
 *  Add a level of indentation to the current thread.
 */
void Logging::Indent()
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  this->indent_[pthread_self()]++;
  return ;
}

/**
 *  Signal when a thread is over so that allocated ressources are freed.
 */
void Logging::ThreadOver()
{
  boost::unique_lock<boost::mutex> lock(this->mutex_);
  std::map<pthread_t, int>::iterator it;

  it = this->indent_.find(pthread_self());
  if (it != this->indent_.end())
    this->indent_.erase(it);
  return ;
}
