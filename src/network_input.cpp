/*
** network_input.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#include <boost/thread/mutex.hpp>
#include "event_publisher.h"
#include "host_status.h"
#include "network_input.h"
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

  do
    {
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
      this->buffer_[this->discard_] = '\0';
      this->discard_++;
    } while (!this->buffer_[0]); // Empty string
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
 *  cleanup function to run on termination.
 */
static CentreonBroker::NetworkInputDestructionRegistration gl_nidr;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

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
  char* buffer;
  const char* types = ">>>>SSSsssssttsttstttsttsssssssssdddssssiSSddi";
  static void (HostStatus::* const set_double[])(double) =
    {
      &HostStatus::SetPercentStateChange,
      &HostStatus::SetLatency,
      &HostStatus::SetExecutionTime,
      &HostStatus::SetNormalCheckInterval,
      &HostStatus::SetRetryCheckInterval
    };
  int cur_set_double;
  static void (HostStatus::* const set_int[])(int) =
    {
      &HostStatus::SetModifiedAttributes,
      &HostStatus::SetCheckTimeperiodObjectId
    };
  int cur_set_int;
  static void (HostStatus::* const set_short[])(short) =
    {
      &HostStatus::SetCurrentState,
      &HostStatus::SetHasBeenChecked,
      &HostStatus::SetShouldBeScheduled,
      &HostStatus::SetCurrentCheckAttempt,
      &HostStatus::SetMaxCheckAttempts,
      &HostStatus::SetCheckType,
      &HostStatus::SetLastHardState,
      &HostStatus::SetStateType,
      &HostStatus::SetNoMoreNotifications,
      &HostStatus::SetProblemHasBeenAcknowledged,
      &HostStatus::SetAcknowledgementType,
      &HostStatus::SetCurrentNotificationNumber,
      &HostStatus::SetPassiveChecksEnabled,
      &HostStatus::SetEventHandlerEnabled,
      &HostStatus::SetActiveChecksEnabled,
      &HostStatus::SetFlapDetectionEnabled,
      &HostStatus::SetIsFlapping,
      &HostStatus::SetScheduledDowntimeDepth,
      &HostStatus::SetFailurePredictionEnabled,
      &HostStatus::SetProcessPerformanceData,
      &HostStatus::SetObsessOver
    };
  int cur_set_short;
  static void (HostStatus::* const set_str[])(const std::string&) =
    {
      &HostStatus::SetHost,
      &HostStatus::SetOutput,
      &HostStatus::SetPerfdata,
      &HostStatus::SetEventHandler,
      &HostStatus::SetCheckCommand
    };
  int cur_set_str;
  static void (HostStatus::* const set_timet[])(time_t) =
    {
      &HostStatus::SetLastCheck,
      &HostStatus::SetNextCheck,
      &HostStatus::SetLastStateChange,
      &HostStatus::SetLastHardStateChange,
      &HostStatus::SetLastTimeUp,
      &HostStatus::SetLastTimeDown,
      &HostStatus::SetLastTimeUnreachable,
      &HostStatus::SetLastNotification,
      &HostStatus::SetNextNotification
    };
  int cur_set_timet;

  cur_set_double = cur_set_int = cur_set_short = cur_set_str = cur_set_timet = 0;
  buffer = socket.GetLine();
  if (strcmp(buffer, "999"))
    {
      HostStatus* hse;

      hse = new HostStatus;
      hse->SetNagiosInstance(this->instance_);
      for (int i = 0; types[i]; i++)
        {
          switch (types[i])
            {
             case 'd': // double
              (hse->*set_double[cur_set_double++])(strtod(strchr(buffer, '=') + 1, NULL));
              break ;
             case 'i': // int
              (hse->*set_int[cur_set_int++])(atoi(strchr(buffer, '=') + 1));
              break ;
             case 's': // short
              (hse->*set_short[cur_set_short++])(atoi(strchr(buffer, '=') + 1));
              break ;
             case 'S': // string
              (hse->*set_str[cur_set_str++])(strchr(buffer, '=') + 1);
              break ;
             case 't': // time_t
              (hse->*set_timet[cur_set_timet++])(atoi(strchr(buffer, '=') + 1));
              break ;
             case '>': // skip
              break ;
             default:
              assert(false);
            }
          buffer = socket.GetLine();
        }
      EventPublisher::GetInstance()->Publish(hse);
    }
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
 *  This is the thread entry point.
 */
void NetworkInput::operator()()
{
  char* buffer;
  ProtocolSocket socket(this->socket_);

  try
    {
      while (1)
	{
	  buffer = socket.GetLine();
	  if (!strcmp(buffer, "212:"))
	    this->HandleHostStatus(socket);
	}
    }
  catch (...)
    {
      // XXX : some error message, somewhere
    }
  delete (this);
  return ;
}
