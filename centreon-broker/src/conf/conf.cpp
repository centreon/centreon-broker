/*
** conf.cpp for CentreonBroker in ./src/conf
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/17/09 Matthieu Kermagoret
** Last update 06/17/09 Matthieu Kermagoret
*/

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "conf/conf.h"
#include "exception.h"
#include "logging.h"

using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Handle the definition of an input.
 */
void Conf::HandleInput(std::ifstream& ifs)
{
  char buffer[2048];
  Input in;

#ifndef NDEBUG
  logging.AddDebug("Input definition...");
#endif /* !NDEBUG */
  ifs.getline(buffer, sizeof(buffer));
  while (ifs.good())
    {
      char* key;
      char* value;

      key = buffer + strspn(buffer, " \t");
      if ('}' == key[0])
	break ;
      value = strchr(key, '=');
      if (value)
	{
	  *value = '\0';
	  value++;
	}

      if ('#' == key[0])
	; // Skip line
      else if (!strcmp(key, "port"))
	in.SetPort(value ? strtol(value, NULL, 0) : 0);
      else if (!strcmp(key, "tls"))
	in.SetTls(value ? !strcmp(value, "yes") : false);
      else if (!strcmp(key, "type"))
	in.SetType(value ? value : "");
      ifs.getline(buffer, sizeof(buffer));
    }
  this->inputs_.push_back(in);
  return ;
}

/**
 *  Handle the definition of an output.
 */
void Conf::HandleOutput(std::ifstream& ifs)
{
  char buffer[2048];
  Output out;

#ifndef NDEBUG
  logging.AddDebug("Output definition");
#endif /* !NDEBUG */
  ifs.getline(buffer, sizeof(buffer));
  while (ifs.good())
    {
      char* key;
      char* value;

      key = buffer + strspn(buffer, " \t");
      if ('}' == key[0])
	break ;
      value = strchr(key, '=');
      if (value)
	{
	  *value = '\0';
	  value++;
	}

      if ('#' == key[0])
	; // Skip line
      else if (!strcmp(key, "db"))
	out.SetDb(value ? value : "");
      else if (!strcmp(key, "host"))
	out.SetHost(value ? value : "");
      else if (!strcmp(key, "password"))
	out.SetPassword(value ? value : "");
      else if (!strcmp(key, "prefix"))
	out.SetPrefix(value ? value : "");
      else if (!strcmp(key, "user"))
	out.SetUser(value ? value : "");
      else if (!strcmp(key, "type"))
	out.SetType(value ? value : "");
      ifs.getline(buffer, sizeof(buffer));
    }
  this->outputs_.push_back(out);
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Conf default constructor.
 */
Conf::Conf()
{
}

/**
 *  Conf copy constructor.
 */
Conf::Conf(const Conf& conf)
{
  this->operator=(conf);
}

/**
 *  Conf destructor.
 */
Conf::~Conf()
{
}

/**
 *  Conf operator= overload.
 */
Conf& Conf::operator=(const Conf& conf)
{
  this->filename_ = conf.filename_;
  this->inputs_ = conf.inputs_;
  this->in_it_ = this->inputs_.begin();
  this->outputs_ = conf.outputs_;
  this->out_it_ = this->outputs_.begin();
  this->params_ = conf.params_;
  return (*this);
}

/**
 *  Return an integer parameter.
 */
int Conf::GetIntegerParam(const std::string& param) const
{
  std::map<std::string, std::string>::const_iterator it;
  int return_value;

  it = this->params_.find(param);
  if (it == this->params_.end())
    return_value = 0;
  else
    return_value = strtol(((*it).second).c_str(), NULL, 0);
  return (return_value);
}

/**
 *  Return a string parameter.
 */
const std::string* Conf::GetStringParam(const std::string& param) const
{
  std::map<std::string, std::string>::const_iterator it;
  const std::string* return_value;

  it = this->params_.find(param);
  if (it == this->params_.end())
    return_value = NULL;
  else
    return_value = &((*it).second);
  return (return_value);
}

/**
 *  Return the next input.
 */
const Input* Conf::GetNextInput() const
{
  const Input* return_value;

  if (this->in_it_ == this->inputs_.end())
    {
      return_value = NULL;
      this->in_it_ = this->inputs_.begin();
    }
  else
    {
      return_value = &(*this->in_it_);
      this->in_it_++;
    }
  return (return_value);
}

/**
 *  Return the next output.
 */
const Output* Conf::GetNextOutput() const
{
  const Output* return_value;

  if (this->out_it_ == this->outputs_.end())
    {
      return_value = NULL;
      this->out_it_ = this->outputs_.begin();
    }
  else
    {
      return_value = &(*this->out_it_);
      this->out_it_++;
    }
  return (return_value);
}

/**
 *  Load a configuration file.
 */
void Conf::Load(const std::string& filename)
{
  std::ifstream ifs;

#ifndef NDEBUG
  logging.AddDebug("Loading configuration file...");
  logging.Indent();
#endif /* !NDEBUG */
  this->filename_ = filename;
  ifs.open(filename.c_str());
  if (ifs)
    {
      char buffer[1024];

      ifs.getline(buffer, sizeof(buffer));
      while (ifs.good())
	{
	  char* ptr;

	  ptr = buffer + strspn(buffer, " \t");
	  if (buffer[0] == '#')
	    ;
	  else if (!strcmp(ptr, "input"))
	    this->HandleInput(ifs);
	  else if (!strcmp(ptr, "output"))
	    this->HandleOutput(ifs);
	  ifs.getline(buffer, sizeof(buffer));
	}
      ifs.close();
    }
  else
    {
#ifndef NDEBUG
      logging.Deindent();
#endif /* !NDEBUG */
      throw (Exception(0, "Could not load configuration file"));
    }
#ifndef NDEBUG
  logging.Deindent();
#endif /* !NDEBUG */
  this->in_it_ = this->inputs_.begin();
  this->out_it_ = this->outputs_.begin();
  return ;
}

/**
 *  Update the configuration (ie. reparse file).
 */
void Conf::Update()
{
#ifndef NDEBUG
  logging.AddDebug("Reloading configuration file, "\
		   "deleting old configuration...");
#endif /* !NDEBUG */
  this->inputs_.clear();
  this->outputs_.clear();
  this->params_.clear();
  this->Load(std::string(filename_));
  this->in_it_ = this->inputs_.begin();
  this->out_it_ = this->outputs_.begin();
  return ;
}
