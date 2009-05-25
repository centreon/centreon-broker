/*
** host.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 05/25/09 Matthieu Kermagoret
*/

#include <cstring>
#include "host.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the Host object to the current instance.
 */
void Host::InternalCopy(const Host& h)
{
  memcpy(this->shorts_, h.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = h.strings_[i];
  memcpy(this->timets_, h.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Host default constructor.
 */
Host::Host() throw ()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Host copy constructor.
 */
Host::Host(const Host& h) : HostService(h), HostServiceStatus(h)
{
  this->InternalCopy(h);
}

/**
 *  Host destructor.
 */
Host::~Host() throw ()
{
}

/**
 *  Host operator= overload.
 */
Host& Host::operator=(const Host& h)
{
  this->HostService::operator=(h);
  this->HostServiceStatus::operator=(h);
  this->InternalCopy(h);
  return (*this);
}

/**
 *  Get the address member.
 */
const std::string& Host::GetAddress() const throw ()
{
  return (this->strings_[ADDRESS]);
}

/**
 *  Get the alias member.
 */
const std::string& Host::GetAlias() const throw ()
{
  return (this->strings_[ALIAS]);
}

/**
 *  Get the flap_detection_on_down member.
 */
short Host::GetFlapDetectionOnDown() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_DOWN]);
}

/**
 *  Get the flap_detection_on_unreachable member.
 */
short Host::GetFlapDetectionOnUnreachable() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_UNREACHABLE]);
}

/**
 *  Get the flap_detection_on_up member.
 */
short Host::GetFlapDetectionOnUp() const throw ()
{
  return (this->shorts_[FLAP_DETECTION_ON_UP]);
}

/**
 *  Get the have_2d_coords member.
 */
short Host::GetHave2DCoords() const throw ()
{
  return (this->shorts_[HAVE_2D_COORDS]);
}

/**
 *  Get the last_time_down member.
 */
time_t Host::GetLastTimeDown() const throw ()
{
  return (this->timets_[LAST_TIME_DOWN]);
}

/**
 *  Get the last_time_unreachable member.
 */
time_t Host::GetLastTimeUnreachable() const throw ()
{
  return (this->timets_[LAST_TIME_UNREACHABLE]);
}

/**
 *  Get the last_time_up member.
 */
time_t Host::GetLastTimeUp() const throw ()
{
  return (this->timets_[LAST_TIME_UP]);
}

/**
 *  Get the notify_on_down member.
 */
short Host::GetNotifyOnDown() const throw ()
{
  return (this->shorts_[NOTIFY_ON_DOWN]);
}

/**
 *  Get the notify_on_unreachable member.
 */
short Host::GetNotifyOnUnreachable() const throw ()
{
  return (this->shorts_[NOTIFY_ON_UNREACHABLE]);
}

/**
 *  Get the stalk_on_down member.
 */
short Host::GetStalkOnDown() const throw ()
{
  return (this->shorts_[STALK_ON_DOWN]);
}

/**
 *  Get the stalk_on_unreachable member.
 */
short Host::GetStalkOnUnreachable() const throw ()
{
  return (this->shorts_[STALK_ON_UNREACHABLE]);
}

/**
 *  Get the stalk_on_up member.
 */
short Host::GetStalkOnUp() const throw ()
{
  return (this->shorts_[STALK_ON_UP]);
}

/**
 *  Get the statusmap_image member.
 */
const std::string& Host::GetStatusmapImage() const throw ()
{
  return (this->strings_[STATUSMAP_IMAGE]);
}

/**
 *  Get the type of the event.
 */
int Host::GetType() const throw ()
{
  // XXX : hardcoded value
  return (9);
}

/**
 *  Get the vrml_image member.
 */
const std::string& Host::GetVrmlImage() const throw ()
{
  return (this->strings_[VRML_IMAGE]);
}

/**
 *  Get the x_2d member.
 */
short Host::GetX2D() const throw ()
{
  return (this->shorts_[X_2D]);
}

/**
 *  Get the y_2d member.
 */
short Host::GetY2D() const throw ()
{
  return (this->shorts_[Y_2D]);
}

/**
 *  Set the address member.
 */
void Host::SetAddress(const std::string& a)
{
  this->strings_[ADDRESS] = a;
  return ;
}

/**
 *  Set the alias member.
 */
void Host::SetAlias(const std::string& a)
{
  this->strings_[ALIAS] = a;
  return ;
}

/**
 *  Set the flap_detection_on_down member.
 */
void Host::SetFlapDetectionOnDown(short fdod) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_DOWN] = fdod;
  return ;
}

/**
 *  Set the flap_detection_on_unreachable member.
 */
void Host::SetFlapDetectionOnUnreachable(short fdou) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_UNREACHABLE] = fdou;
  return ;
}

/**
 *  Set the flap_detection_on_up member.
 */
void Host::SetFlapDetectionOnUp(short fdou) throw ()
{
  this->shorts_[FLAP_DETECTION_ON_UP] = fdou;
  return ;
}

/**
 *  Set the have_2d_coords member.
 */
void Host::SetHave2DCoords(short h2dc) throw ()
{
  this->shorts_[HAVE_2D_COORDS] = h2dc;
  return ;
}

/**
 *  Set the last_time_down member.
 */
void Host::SetLastTimeDown(time_t ltd) throw ()
{
  this->timets_[LAST_TIME_DOWN] = ltd;
  return ;
}

/**
 *  Set the last_time_unreachable member.
 */
void Host::SetLastTimeUnreachable(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UNREACHABLE] = ltu;
  return ;
}

/**
 *  Set the last_time_up member.
 */
void Host::SetLastTimeUp(time_t ltu) throw ()
{
  this->timets_[LAST_TIME_UP] = ltu;
  return ;
}

/**
 *  Set the notify_on_down member.
 */
void Host::SetNotifyOnDown(short nod) throw ()
{
  this->shorts_[NOTIFY_ON_DOWN] = nod;
  return ;
}

/**
 *  Set the notify_on_unreachable member.
 */
void Host::SetNotifyOnUnreachable(short nou) throw ()
{
  this->shorts_[NOTIFY_ON_UNREACHABLE] = nou;
  return ;
}

/**
 *  Set the stalk_on_down member.
 */
void Host::SetStalkOnDown(short sod) throw ()
{
  this->shorts_[STALK_ON_DOWN] = sod;
  return ;
}

/**
 *  Set the stalk_on_unreachable member.
 */
void Host::SetStalkOnUnreachable(short sou) throw ()
{
  this->shorts_[STALK_ON_UNREACHABLE] = sou;
  return ;
}

/**
 *  Set the stalk_on_up member.
 */
void Host::SetStalkOnUp(short sou) throw ()
{
  this->shorts_[STALK_ON_UP] = sou;
  return ;
}

/**
 *  Set the statusmap_image member.
 */
void Host::SetStatusmapImage(const std::string& si)
{
  this->strings_[STATUSMAP_IMAGE] = si;
  return ;
}

/**
 *  Set the vrml_image member.
 */
void Host::SetVrmlImage(const std::string& vi)
{
  this->strings_[VRML_IMAGE] = vi;
  return ;
}

/**
 *  Set the x_2d member.
 */
void Host::SetX2D(short x_2d) throw ()
{
  this->shorts_[X_2D] = x_2d;
  return ;
}

/**
 *  Set the y_2d member.
 */
void Host::SetY2D(short y_2d) throw ()
{
  this->shorts_[Y_2D] = y_2d;
  return ;
}
