/*
** logging.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/12/09 Matthieu Kermagoret
** Last update 06/22/09 Matthieu Kermagoret
*/

#ifndef LOGGING_H_
# define LOGGING_H_

# include <boost/thread.hpp>
# include <boost/thread/mutex.hpp>
# include <fstream>
# include <map>
# include <string>
# include <vector>

namespace               CentreonBroker
{
  class                 Logging
  {
   public:
    enum
    {
      DEBUG = 1,
      ERROR = 2,
      INFO = 4
    };

   private:
    class               Output
    {
     private:
      void              Clean();
      void              InternalCopy(const Logging::Output& output);

     public:
      std::string       filename_;
      int               flags_;
      std::ofstream*    stream_;

                        Output();
                        Output(const Output& output);
                        ~Output() throw ();
      Output&           operator=(const Output& output);
    };
    std::map<boost::thread::id, int>
                        indent_;
    boost::mutex        mutex_;
    std::vector<Output> outputs_;
    int                 syslog_flags_;
    bool                use_syslog_;
                        Logging(const Logging& logging);
    Logging&            operator=(const Logging& logging);
    void                LogBase(const char* str, int flags, bool indent)
                          throw ();

   public:
                        Logging();
                        ~Logging() throw ();
    void                Deindent() throw ();
    void                Indent() throw ();
# ifndef NDEBUG
    void                LogDebug(const char* str, bool indent = false)
                          throw ();
# endif /* !NDEBUG */
    void                LogError(const char* str, bool indent = false)
                          throw ();
    void                LogInFile(const char* filename, int log_flags);
    void                LogInSyslog(bool use_syslog, int log_flags) throw ();
    void                LogInfo(const char* str, bool indent = false) throw ();
    void                ThreadEnd();
    void                ThreadStart();
  };

  extern Logging        logging;
}

#endif /* !LOGGING_H_ */
