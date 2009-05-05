/*
** fileoutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef FILEOUTPUT_H_
# define FILEOUTPUT_H_

# include <string>
# include "exception.h"
# include "iomanager.h"

namespace       CentreonBroker
{
  class         FileOutput
    : public CentreonBroker::ErrorManager,
    public CentreonBroker::WriteManager
  {
   private:
    int         fd;
    std::string buffer;
                FileOutput(const FileOutput& fileo);
    FileOutput& operator=(const FileOutput& fileo);

   public:
                FileOutput();
                FileOutput(const std::string& filename);
                ~FileOutput();
    void        Close();
    void        Open(const std::string& filename)
                  throw (CentreonBroker::Exception);

    // ErrorManager
    void        OnError(int fd);

    // WriteManager
    bool        IsWaitingToWrite() const;
    void        OnWrite(int fd);

    // XXX : handle events
  };
}

#endif /* !FILEOUTPUT_H_ */
