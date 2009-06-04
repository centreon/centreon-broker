/*
** network_input.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include <boost/thread/mutex.hpp>
#include <cstdlib>
#include "event_publisher.h"
#include "host_status.h"
#include "nagios/protoapi.h"
#include "network_input.h"
#include "program_status.h"
#include "service_status.h"

using namespace CentreonBroker;

/******************************************************************************
*                                                                             *
*                                                                             *
*                              ProtocolSocket                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*              Definition             *
*                                     *
**************************************/

/**
 *  The ProtocolSocket class will buffer input from the socket and make it
 *  available a line at a time.
 */
namespace                         CentreonBroker
{
  class                           ProtocolSocket
  {
   private:
    char                          buffer_[1024];
    size_t                        discard_;
    size_t                        length_;
    boost::asio::ip::tcp::socket& socket_;
    ProtocolSocket&               operator=(const ProtocolSocket& ps) throw ();

   public:
                                  ProtocolSocket(
                                    boost::asio::ip::tcp::socket& socket)
                                    throw ();
                                  ProtocolSocket(const ProtocolSocket& ps)
                                   throw ();
                                  ~ProtocolSocket() throw ();
    char*                         GetLine();
  };
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ProtocolSocket operator= overload.
 */
ProtocolSocket& ProtocolSocket::operator=(const ProtocolSocket& ps) throw ()
{
  (void)ps;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ProtocolSocket constructor.
 */
ProtocolSocket::ProtocolSocket(boost::asio::ip::tcp::socket& s)
  throw () : socket_(s)
{
  this->discard_ = 0;
  this->length_ = 0;
}

/**
 *  ProtocolSocket copy constructor.
 */
ProtocolSocket::ProtocolSocket(const ProtocolSocket& ps) throw ()
  : socket_(ps.socket_)
{
  memcpy(this->buffer_, ps.buffer_, sizeof(this->buffer_));
  this->discard_ = ps.discard_;
  this->length_ = ps.length_;
}

/**
 *  ProtocolSocket destructor.
 */
ProtocolSocket::~ProtocolSocket() throw ()
{
}

/**
 *  Fetch a line of input.
 */
char* ProtocolSocket::GetLine()
{
  int old_length;

  this->length_ -= this->discard_;
  memmove(this->buffer_,
          this->buffer_ + this->discard_,
          this->length_ + 1);
  this->discard_ = 0;
  old_length = 0;
  while (!strchr(this->buffer_ + old_length, '\n')
         && this->length_ < sizeof(this->buffer_) - 1)
    {
      old_length = this->length_;
      this->length_ += this->socket_.receive(
                         boost::asio::buffer(this->buffer_ + this->length_,
                           sizeof(this->buffer_) - this->length_ - 1));
      this->buffer_[this->length_] = '\0';
    }
  this->discard_ = strcspn(this->buffer_, "\n");
  this->buffer_[this->discard_++] = '\0';
  return (this->buffer_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               NetworkInput                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Static objects            *
*                                     *
**************************************/

static boost::mutex             gl_mutex;
static std::list<NetworkInput*> gl_ni;

static void networkinput_destruction() throw ()
{
  try
    {
      boost::unique_lock<boost::mutex> lock(gl_mutex);

      while (!gl_ni.empty())
        {
          std::list<NetworkInput*>::iterator it;

          it = gl_ni.begin();
          lock.unlock();
          delete (*it);
          lock.lock();
        }
    }
  catch (...)
    {
    }
  return ;
}

namespace    CentreonBroker
{
  struct     NetworkInputDestructionRegistration
  {
             NetworkInputDestructionRegistration() throw ()
    {
      atexit(networkinput_destruction);
    }
  };
}

/**
 *  This fake static object is used to register at startup the NetworkInput
 *  cleanup function that will run on termination.
 */
static CentreonBroker::NetworkInputDestructionRegistration gl_nidr;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  This structure is used by the HandleObject template function.
 */
template <typename Event>
struct KeySetter
{
  int key;
  char type;
  union UHandler
  {
   public:
    void (Event::* set_double)(double);
    void (Event::* set_int)(int);
    void (Event::* set_short)(short);
    void (Event::* set_string)(const std::string&);
    void (Event::* set_timet)(time_t);

    UHandler() : set_double(NULL) {}
    UHandler(void (Event::* sd)(double)) : set_double(sd) {}
    UHandler(void (Event::* si)(int)) : set_int(si) {}
    UHandler(void (Event::* ss)(short)) : set_short(ss) {}
    UHandler(void (Event::* ss)(const std::string&)) : set_string(ss) {}
    UHandler(void (Event::* st)(time_t)) : set_timet(st) {}
  } setter;
};

/**
 *  For all kind of events, this template function will parse the socket input,
 *  fill the object and publish it.
 */
template <typename Event>
static inline void HandleObject(const std::string& instance,
                                const KeySetter<Event>* key_setters,
                                ProtocolSocket& ps)
{
  int key;
  char* key_str;
  const char* value_str;
  char* tmp;
  Event* event;

  event = new Event;
  event->SetNagiosInstance(instance);
  key_str = ps.GetLine();
  tmp = strchr(key_str, '=');
  if (!tmp)
    value_str = "";
  else
    {
      *tmp = '\0';
      value_str = tmp + 1;
    }
  key = strtol(key_str, NULL, 0);
  while (key != NDO_API_ENDDATA)
    {
      for (unsigned int i = 0; key_setters[i].key; i++)
        if (key == key_setters[i].key)
          {
            switch (key_setters[i].type)
              {
               case 'd':
                (event->*key_setters[i].setter.set_double)(strtod(value_str,
                                                                  NULL));
                break ;
               case 'i':
                (event->*key_setters[i].setter.set_int)(strtol(value_str,
                                                               NULL,
                                                               0));
                break ;
               case 's':
                (event->*key_setters[i].setter.set_short)(strtol(value_str,
                                                                 NULL,
                                                                 0));
                break ;
               case 'S':
                (event->*key_setters[i].setter.set_string)(value_str);
                break ;
               case 't':
                (event->*key_setters[i].setter.set_timet)(strtol(value_str,
                                                                 NULL,
                                                                 0));
                break ;
               default:
                assert(false);
	      }
	    break ;
	  }
      key_str = ps.GetLine();
      tmp = strchr(key_str, '=');
      if (!tmp)
	value_str = "";
      else
	{
	  *tmp = '\0';
	  value_str = tmp + 1;
	}
      key = strtol(key_str, NULL, 0);
    }
  EventPublisher::GetInstance()->Publish(event);
  return ;
}

/**
 *  NetworkInput constructor.
 */
NetworkInput::NetworkInput(boost::asio::ip::tcp::socket& socket)
  : socket_(socket)
{
  this->thread_ = new boost::thread(boost::ref(*this));
  this->thread_->detach();
}

/**
 *  NetworkInput copy constructor.
 */
NetworkInput::NetworkInput(const NetworkInput& ni) : socket_(ni.socket_)
{
  (void)ni;
}

/**
 *  NetworkInput operator= overload.
 */
NetworkInput& NetworkInput::operator=(const NetworkInput& ni)
{
  (void)ni;
  return (*this);
}

/**
 *  Handle a host status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleHostStatus(ProtocolSocket& socket)
{
  static const KeySetter<HostStatus> keys_setters[] =
      {
	{ NDO_DATA_ACKNOWLEDGEMENTTYPE,
          's',
          &HostStatus::SetAcknowledgementType },
	{ NDO_DATA_ACTIVEHOSTCHECKSENABLED,
          's',
          &HostStatus::SetActiveChecksEnabled },
	{ NDO_DATA_CHECKCOMMAND, 'S', &HostStatus::SetCheckCommand },
	{ NDO_DATA_CHECKTYPE, 's', &HostStatus::SetCheckType },
	{ NDO_DATA_CURRENTCHECKATTEMPT,
          's',
          &HostStatus::SetCurrentCheckAttempt },
	{ NDO_DATA_CURRENTNOTIFICATIONNUMBER,
	  's',
	  &HostStatus::SetCurrentNotificationNumber },
	{ NDO_DATA_CURRENTSTATE, 's', &HostStatus::SetCurrentState },
	{ NDO_DATA_EVENTHANDLER, 'S', &HostStatus::SetEventHandler },
	{ NDO_DATA_EVENTHANDLERENABLED,
          's',
          &HostStatus::SetEventHandlerEnabled },
	{ NDO_DATA_EXECUTIONTIME, 'd', &HostStatus::SetExecutionTime },
	{ NDO_DATA_FAILUREPREDICTIONENABLED,
	  's',
	  &HostStatus::SetFailurePredictionEnabled },
	{ NDO_DATA_FLAPDETECTIONENABLED,
          's',
          &HostStatus::SetFlapDetectionEnabled },
	{ NDO_DATA_HASBEENCHECKED, 's', &HostStatus::SetHasBeenChecked },
	{ NDO_DATA_HOST, 'S', &HostStatus::SetHostName },
	{ NDO_DATA_ISFLAPPING, 's', &HostStatus::SetIsFlapping },
	{ NDO_DATA_LASTHARDSTATE, 's', &HostStatus::SetLastHardState },
	{ NDO_DATA_LASTHARDSTATECHANGE,
          't',
          &HostStatus::SetLastHardStateChange },
	{ NDO_DATA_LASTHOSTCHECK, 't', &HostStatus::SetLastCheck },
	{ NDO_DATA_LASTHOSTNOTIFICATION,
          't',
          &HostStatus::SetLastNotification },
	{ NDO_DATA_LASTSTATECHANGE, 't', &HostStatus::SetLastStateChange },
	{ NDO_DATA_LASTTIMEDOWN, 't', &HostStatus::SetLastTimeDown },
	{ NDO_DATA_LASTTIMEUNREACHABLE,
          't',
          &HostStatus::SetLastTimeUnreachable},
	{ NDO_DATA_LATENCY, 'd', &HostStatus::SetLatency },
	{ NDO_DATA_MAXCHECKATTEMPTS, 's', &HostStatus::SetMaxCheckAttempts },
	{ NDO_DATA_MODIFIEDHOSTATTRIBUTES,
          'i',
          &HostStatus::SetModifiedAttributes },
	{ NDO_DATA_NEXTHOSTCHECK, 't', &HostStatus::SetNextCheck },
	{ NDO_DATA_NEXTHOSTNOTIFICATION,
          't',
          &HostStatus::SetNextNotification },
	{ NDO_DATA_NOMORENOTIFICATIONS,
          's',
          &HostStatus::SetNoMoreNotifications },
	{ NDO_DATA_NORMALCHECKINTERVAL,
	  'd',
	  &HostStatus::SetCheckInterval },
	{ NDO_DATA_NOTIFICATIONSENABLED,
          's',
          &HostStatus::SetNotificationsEnabled },
	{ NDO_DATA_OBSESSOVERHOST, 's', &HostStatus::SetObsessOver },
	{ NDO_DATA_OUTPUT, 'S', &HostStatus::SetOutput },
	{ NDO_DATA_PASSIVEHOSTCHECKSENABLED,
	  's',
	  &HostStatus::SetPassiveChecksEnabled },
	{ NDO_DATA_PERCENTSTATECHANGE,
          'd',
          &HostStatus::SetPercentStateChange },
	{ NDO_DATA_PERFDATA, 'S', &HostStatus::SetPerfdata },
	{ NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
          's',
          &HostStatus::SetProblemHasBeenAcknowledged },
	{ NDO_DATA_PROCESSPERFORMANCEDATA,
	  's',
	  &HostStatus::SetProcessPerformanceData },
	{ NDO_DATA_RETRYCHECKINTERVAL,
	  'd',
	  &HostStatus::SetRetryInterval },
	{ NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
          's',
          &HostStatus::SetScheduledDowntimeDepth },
	{ NDO_DATA_SHOULDBESCHEDULED, 's', &HostStatus::SetShouldBeScheduled },
	{ NDO_DATA_STATETYPE, 's', &HostStatus::SetStateType },
	{ 0, '\0', static_cast<void (HostStatus::*)(double)>(NULL) }
      };

  HandleObject(this->instance_, keys_setters, socket);
  return ;
}

/**
 *  Handle a program status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleProgramStatus(ProtocolSocket& ps)
{
  static const KeySetter<ProgramStatus> keys_setters[] =
    {
      { NDO_DATA_ACTIVEHOSTCHECKSENABLED,
	's',
	&ProgramStatus::SetActiveHostChecksEnabled },
      { NDO_DATA_ACTIVESERVICECHECKSENABLED,
	's',
	&ProgramStatus::SetActiveServiceChecksEnabled },
      { NDO_DATA_DAEMONMODE, 's', &ProgramStatus::SetDaemonMode },
      { NDO_DATA_EVENTHANDLERENABLED,
        's',
        &ProgramStatus::SetEventHandlerEnabled },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	's',
	&ProgramStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	's',
	&ProgramStatus::SetFlapDetectionEnabled },
      { NDO_DATA_GLOBALHOSTEVENTHANDLER,
	'S',
	&ProgramStatus::SetGlobalHostEventHandler },
      { NDO_DATA_GLOBALSERVICEEVENTHANDLER,
	'S',
	&ProgramStatus::SetGlobalServiceEventHandler },
      { NDO_DATA_LASTCOMMANDCHECK,
        't',
        &ProgramStatus::SetLastCommandCheck },
      { NDO_DATA_LASTLOGROTATION,
	't',
	&ProgramStatus::SetLastLogRotation },
      { NDO_DATA_MODIFIEDHOSTATTRIBUTES,
	'i',
	&ProgramStatus::SetModifiedHostAttributes },
      { NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
	'i',
	&ProgramStatus::SetModifiedServiceAttributes },
      { NDO_DATA_NOTIFICATIONSENABLED,
        's',
        &ProgramStatus::SetNotificationsEnabled },
      { NDO_DATA_OBSESSOVERHOST, 's', &ProgramStatus::SetObsessOverHost },
      { NDO_DATA_OBSESSOVERSERVICE,
        's',
        &ProgramStatus::SetObsessOverService },
      { NDO_DATA_PASSIVEHOSTCHECKSENABLED,
	's',
	&ProgramStatus::SetPassiveHostChecksEnabled },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	's',
	&ProgramStatus::SetPassiveServiceChecksEnabled },
      { NDO_DATA_PROCESSPERFORMANCEDATA,
	's',
	&ProgramStatus::SetProcessPerformanceData },
      { NDO_DATA_PROCESSID, 'i', &ProgramStatus::SetProcessId },
      { NDO_DATA_PROGRAMSTARTTIME,
	't',
	&ProgramStatus::SetProgramStartTime },
    };

  HandleObject(this->instance_, keys_setters, ps);
  return ;
}

/**
 *  Handle a service status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleServiceStatus(ProtocolSocket& ps)
{
  static const KeySetter<ServiceStatus> keys_setters[] =
    {
      { NDO_DATA_ACKNOWLEDGEMENTTYPE,
	's',
	&ServiceStatus::SetAcknowledgementType },
      { NDO_DATA_ACTIVESERVICECHECKSENABLED,
	's',
	&ServiceStatus::SetActiveChecksEnabled },
      { NDO_DATA_CHECKCOMMAND, 'S', &ServiceStatus::SetCheckCommand },
      { NDO_DATA_CHECKTYPE, 's', &ServiceStatus::SetCheckType },
      { NDO_DATA_CURRENTCHECKATTEMPT,
        's',
        &ServiceStatus::SetCurrentCheckAttempt },
      { NDO_DATA_CURRENTNOTIFICATIONNUMBER,
	's',
	&ServiceStatus::SetCurrentNotificationNumber },
      { NDO_DATA_CURRENTSTATE, 's', &ServiceStatus::SetCurrentState },
      { NDO_DATA_EVENTHANDLER, 'S', &ServiceStatus::SetEventHandler },
      { NDO_DATA_EVENTHANDLERENABLED,
        's',
        &ServiceStatus::SetEventHandlerEnabled },
      { NDO_DATA_EXECUTIONTIME, 'd', &ServiceStatus::SetExecutionTime },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	's',
	&ServiceStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	's',
	&ServiceStatus::SetFlapDetectionEnabled },
      { NDO_DATA_HASBEENCHECKED, 's', &ServiceStatus::SetHasBeenChecked },
      { NDO_DATA_HOST, 'S', &ServiceStatus::SetHostName },
      { NDO_DATA_ISFLAPPING, 's', &ServiceStatus::SetIsFlapping },
      { NDO_DATA_LASTSERVICECHECK, 't', &ServiceStatus::SetLastCheck },
      { NDO_DATA_LASTHARDSTATE, 's', &ServiceStatus::SetLastHardState },
      { NDO_DATA_LASTHARDSTATECHANGE,
	't',
	&ServiceStatus::SetLastHardStateChange },
      { NDO_DATA_LASTSERVICENOTIFICATION,
        't',
        &ServiceStatus::SetLastNotification },
      { NDO_DATA_LASTSTATECHANGE, 't', &ServiceStatus::SetLastStateChange },
      { NDO_DATA_LASTTIMECRITICAL, 't', &ServiceStatus::SetLastTimeCritical },
      { NDO_DATA_LASTTIMEOK, 't', &ServiceStatus::SetLastTimeOk },
      { NDO_DATA_LASTTIMEUNKNOWN, 't', &ServiceStatus::SetLastTimeUnknown },
      { NDO_DATA_LASTTIMEWARNING, 't', &ServiceStatus::SetLastTimeWarning },
      { NDO_DATA_LATENCY, 'd', &ServiceStatus::SetLatency },
      { NDO_DATA_MAXCHECKATTEMPTS,
	's',
	&ServiceStatus::SetMaxCheckAttempts },
      { NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
	'i',
	&ServiceStatus::SetModifiedAttributes },
      { NDO_DATA_NEXTSERVICECHECK, 't', &ServiceStatus::SetNextCheck },
      { NDO_DATA_NEXTSERVICENOTIFICATION,
	't',
	&ServiceStatus::SetNextNotification },
      { NDO_DATA_NOMORENOTIFICATIONS,
	's',
	&ServiceStatus::SetNoMoreNotifications },
      { NDO_DATA_NORMALCHECKINTERVAL, 'd', &ServiceStatus::SetCheckInterval },
      { NDO_DATA_OBSESSOVERSERVICE, 's', &ServiceStatus::SetObsessOver },
      { NDO_DATA_OUTPUT, 'S', &ServiceStatus::SetOutput },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	's',
	&ServiceStatus::SetPassiveChecksEnabled },
      { NDO_DATA_PERCENTSTATECHANGE,
        'd',
        &ServiceStatus::SetPercentStateChange },
      { NDO_DATA_PERFDATA, 'S', &ServiceStatus::SetPerfdata },
      { NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
	's',
	&ServiceStatus::SetProblemHasBeenAcknowledged },
      { NDO_DATA_PROCESSPERFORMANCEDATA,
	's',
	&ServiceStatus::SetProcessPerformanceData },
      { NDO_DATA_RETRYCHECKINTERVAL, 'd', &ServiceStatus::SetRetryInterval },
      { NDO_DATA_SERVICE, 'S', &ServiceStatus::SetServiceDescription },
      { NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
	's',
	&ServiceStatus::SetScheduledDowntimeDepth },
      { NDO_DATA_SHOULDBESCHEDULED,
        's',
        &ServiceStatus::SetShouldBeScheduled },
      { NDO_DATA_STATETYPE, 's', &ServiceStatus::SetStateType },
      { 0, '\0', static_cast<void (ServiceStatus::*)(double)>(NULL) }
    };

  HandleObject(this->instance_, keys_setters, ps);
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  NetworkInput destructor.
 */
NetworkInput::~NetworkInput() throw ()
{
  // We might end up here and the object already been destroyed so let's be
  // extra careful.
  try
    {
      boost::unique_lock<boost::mutex> lock(gl_mutex);
      std::list<NetworkInput*>::iterator it;

      // XXX : use std::algorithm
      for (it = gl_ni.begin(); it != gl_ni.end(); it++)
        if (*it == this)
          {
            gl_ni.erase(it);
            if (this->socket_.is_open())
              {
                boost::system::error_code ec;

                this->socket_.shutdown(
                  boost::asio::ip::tcp::socket::shutdown_both,
                  ec);
                this->socket_.close(ec);
                delete (&this->socket_);
              }
            break ;
          }
      delete (this->thread_);
    }
  catch (...)
    {
    }
}

/**
 *  Thread entry point.
 */
void NetworkInput::operator()()
{
  char* buffer;
  ProtocolSocket socket(this->socket_);
  static const struct
  {
    int event;
    void (NetworkInput::* handler)(ProtocolSocket&);
  } handlers[] =
      {
	{ NDO_API_HOSTSTATUSDATA, &NetworkInput::HandleHostStatus },
	{ NDO_API_SERVICESTATUSDATA, &NetworkInput::HandleServiceStatus },
	{ 0, NULL }
      };

  try
    {
      while (1)
	{
	  buffer = socket.GetLine();
	  if (4 == strlen(buffer) && ':' == buffer[3])
	    {
	      int event;

	      buffer[3] = '\0';
	      event = strtol(buffer, NULL, 0);
	      for (unsigned int i = 0; handlers[i].event; i++)
		if (handlers[i].event == event)
		  {
		    (this->*(handlers[i].handler))(socket);
		    break ;
		  }
	    }
	}
    }
  catch (...)
    {
      // XXX : some error message, somewhere
    }
  delete (this);
  return ;
}
