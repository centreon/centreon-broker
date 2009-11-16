/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef FILE_OUTPUT_H_
# define FILE_OUTPUT_H_

# include <boost/thread.hpp>
# include <fstream>
# include <memory>
# include <string>
# include "db/data_member.hpp"
# include "event_subscriber.h"
# include "waitable_list.hpp"

namespace                        CentreonBroker
{
  // Forward declarations
  namespace                      Events
  {
    class                        Event;
  }

  class                          FileOutput : public EventSubscriber
  {
   private:
    WaitableList<Events::Event>  events_;
    volatile bool                exit_;
    unsigned int                 max_size_;
    unsigned int                 next_;
    std::ofstream                ofs_;
    std::string                  path_;
    std::auto_ptr<boost::thread> thread_;
                                 FileOutput(const FileOutput& fo);
    FileOutput&                  operator=(const FileOutput& fo);
    template                     <typename T>
    void                         Dump(const T& event,
                                      const DB::DataMember<T> dm[],
                                      unsigned int& wb);
    void                         FileClose();
    void                         OnEvent(Events::Event* e) throw ();
    void                         OpenNext();

   public:
                                 FileOutput();
                                 ~FileOutput();
    void                         operator()();
    void                         Close();
    void                         Lock();
    void                         Open(const std::string& base_path);
    void                         Run();
    void                         SetMaxSize(unsigned int max_size) throw ();
    void                         StoreEvents(WaitableList<Events::Event>& ev);
    void                         Unlock();
  };
}

#endif /* !FILE_OUTPUT_H_ */
