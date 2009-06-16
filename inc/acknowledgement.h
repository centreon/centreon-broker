/*
** acknowledgement.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/16/09 Matthieu Kermagoret
** Last update 06/16/09 Matthieu Kermagoret
*/

#ifndef ACKNOWLEDGEMENT_H_
# define ACKNOWLEDGEMENT_H_

# include <string>
# include "event.h"

namespace              CentreonBroker
{
  class                Acknowledgement : public Event
  {
   private:
    enum               Short
    {
      ACKNOWLEDGEMENT_TYPE,
      IS_STICKY,
      NOTIFY_CONTACTS,
      PERSISTENT_COMMENT,
      STATE,
      SHORT_NB
    };
    enum               String
    {
      AUTHOR_NAME,
      COMMENT,
      HOST,
      SERVICE,
      STRING_NB
    };
    enum               TimeT
    {
      ENTRY_TIME,
      TIMET_NB
    };
    short              shorts_[SHORT_NB];
    std::string        strings_[STRING_NB];
    time_t             timets_[TIMET_NB];
    void               InternalCopy(const Acknowledgement& ack);

   public:
                       Acknowledgement();
                       Acknowledgement(const Acknowledgement& ack);
                       ~Acknowledgement();
    Acknowledgement&   operator=(const Acknowledgement& ack);
    short              GetAcknowledgementType() const throw ();
    const std::string& GetAuthorName() const throw ();
    const std::string& GetComment() const throw ();
    time_t             GetEntryTime() const throw ();
    const std::string& GetHost() const throw ();
    short              GetIsSticky() const throw ();
    short              GetNotifyContacts() const throw ();
    short              GetPersistentComment() const throw ();
    const std::string& GetService() const throw ();
    short              GetState() const throw ();
    int                GetType() const throw ();
    void               SetAcknowledgementType(short at) throw ();
    void               SetAuthorName(const std::string& an);
    void               SetComment(const std::string& c);
    void               SetEntryTime(time_t et) throw ();
    void               SetHost(const std::string& h);
    void               SetIsSticky(short is) throw ();
    void               SetNotifyContacts(short nc) throw ();
    void               SetPersistentComment(short pc) throw ();
    void               SetService(const std::string& s);
    void               SetState(short s) throw ();
  };
}

#endif /* !ACKNOWLEDGEMENT_H_ */
