/*
** network_acceptor.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/18/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#ifndef NETWORK_ACCEPTOR_H_
# define NETWORK_ACCEPTOR_H_

# include <list>
# include "thread.h"

namespace                    CentreonBroker
{
  class                      NetworkInput;

  class                      NetworkAcceptor : private Thread
  {
   private:
    volatile bool            exit_thread_;
    int                      fd_;
    std::list<NetworkInput*> ni_;
    unsigned short           port_;
    int                      Core();

   public:
                             NetworkAcceptor();
                             NetworkAcceptor(const NetworkAcceptor& na);
                             ~NetworkAcceptor();
    NetworkAcceptor&         operator=(const NetworkAcceptor& na);
    unsigned short           GetPort() const throw ();
    void                     Listen();
    void                     SetPort(unsigned short port) throw ();
  };
}

#endif /* !NETWORK_ACCEPTOR_H_ */
