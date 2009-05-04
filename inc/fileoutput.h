/*
** fileoutput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#ifndef FILEOUTPUT_H_
# define FILEOUTPUT_H_

# include "output.h"

namespace       CentreonBroker
{
  class         FileOutput
    : public CentreonBroker::Output
  {
   private:
    int         fd;
                FileOutput(const FileOutput& fileo);
    FileOutput& operator=(const FileOutput& fileo);

   public:
                FileOutput();
                ~FileOutput();

    // Output methods
    void        Error();
    void        Event(const CentreonBroker::Event& event);
    void        Send();
  };
}

#endif /* !FILEOUTPUT_H_ */
