/*
** iomanager.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#ifndef IOMANAGER_H_
# define IOMANAGER_H_

# include <map>
# include "exception.h"
# include "mutex.h"

namespace                        CentreonBroker
{
  /**
   *  An object that would wait for error on a FD should implement methods of
   *  this interface.
   */
  class                          ErrorManager
  {
   public:
                                 ErrorManager();
                                 ErrorManager(const ErrorManager& em);
    virtual                      ~ErrorManager();
    ErrorManager&                operator=(const ErrorManager& em);
    virtual void                 OnError(int fd) = 0;
  };

  /**
   *  An object that would wait for reading on a FD should implement methods of
   *  this interface.
   */
  class                          ReadManager
  {
   public:
                                 ReadManager();
                                 ReadManager(const ReadManager& em);
    virtual                      ~ReadManager();
    ReadManager&                 operator=(const ReadManager& em);
    virtual void                 OnRead(int fd) = 0;
  };

  /**
   *  An object that would wait for writing on a FD should implement methods of
   *  this interface.
   */
  class                          WriteManager
  {
   public:
                                 WriteManager();
                                 WriteManager(const WriteManager& em);
    virtual                      ~WriteManager();
    WriteManager&                operator=(const WriteManager& em);
    virtual bool                 IsWaitingToWrite() const = 0;
    virtual void                 OnWrite(int fd) = 0;
  };

  /**
   *  This singleton is the object responsible for IO multiplexing.
   */
  class                          IOManager
  {
   private:
    struct                       FDManager
    {
      ErrorManager*              em;
      ReadManager*               rm;
      WriteManager*              wm;
    };
    std::map<int, FDManager>     map;
    CentreonBroker::Mutex        map_mutex;
    static IOManager*            instance;
    static CentreonBroker::Mutex mutex;
                                 IOManager();
                                 IOManager(const IOManager& om);
    IOManager&                   operator=(const IOManager& om);

  public:
                                 ~IOManager();
    static IOManager*            GetInstance();
    void                         SetFD(int fd,
                                       ErrorManager* em,
                                       ReadManager* rm,
                                       WriteManager* wm);
    void                         WaitForEvents()
                                   throw (CentreonBroker::Exception);
  };
}

#endif /* !IOMANAGER_H_ */
