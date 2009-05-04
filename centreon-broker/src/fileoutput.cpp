/*
** fileoutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#include <unistd.h>
#include "fileoutput.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  FileOutput copy constructor.
 */
FileOutput::FileOutput(const FileOutput& fileo) : Output()
{
  (void)fileo;
}

/**
 *  FileOutput operator= overload.
 */
FileOutput& FileOutput::operator=(const FileOutput& fileo)
{
  (void)fileo;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  FileOutput default constructor.
 */
FileOutput::FileOutput()
{
  this->fd = -1;
}

/**
 *  FileOutput destructor.
 */
FileOutput::~FileOutput()
{
  if (this->fd >= 0)
    close(this->fd);
}

/**
 *  This function will be called when an error occured on the file descriptor.
 */
void FileOutput::Error()
{
}

/**
 *  This function will be called when an event as to be written to the file.
 */
void FileOutput::Event(const CentreonBroker::Event& event)
{
}

/**
 *  This function will be called when the FD is available for writing.
 */
void FileOutput::Send()
{
}
