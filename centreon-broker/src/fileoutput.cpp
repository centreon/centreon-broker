/*
** fileoutput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#include <fcntl.h>
#include <sys/stat.h>
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
FileOutput::FileOutput(const FileOutput& fileo)
  : ErrorManager(fileo), WriteManager(fileo)
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
  // XXX : register to IOManager
  this->fd = -1;
}

/**
 *  Build a FileOutput and open a file.
 */
FileOutput::FileOutput(const std::string& filename)
{
  this->Open(filename);
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
 *  Closes the file.
 */
void FileOutput::Close()
{
  close(this->fd);
  this->fd = -1;
  return ;
}

/**
 *  Opens a file.
 */
void FileOutput::Open(const std::string& filename)
  throw (Exception)
{
  this->fd = open(filename.c_str(),
                  O_CREAT | O_WRONLY | O_TRUNC, // XXX : O_TRUNC or O_APPEND ?
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (this->fd < 0)
    throw (Exception("Could not open output file."));
  return ;
}

/**
 *  This function will be called when an error occured on the file descriptor.
 */
void FileOutput::OnError(int fd)
{
  (void)fd;
  // XXX : detect error
  return ;
}

/**
 *  Returns true if we have data to write.
 */
bool FileOutput::IsWaitingToWrite() const
{
  return (this->fd >= 0 && !this->buffer.empty());
}

/**
 *  This function will be called when the FD is available for writing.
 */
void FileOutput::OnWrite(int fd)
{
  ssize_t wb;

  (void)fd;
  wb = write(this->fd, this->buffer.c_str(), this->buffer.size());
  // XXX : detect error
  this->buffer.erase(0, wb);
  return ;
}
