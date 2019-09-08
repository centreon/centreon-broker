/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_TLS_INTERNAL_HH
#define CCB_TLS_INTERNAL_HH

#include <gnutls/gnutls.h>
#include <sys/types.h>

CCB_BEGIN()

namespace tls {
// Data.
extern unsigned char const dh_params_2048[];
extern gnutls_dh_params_t dh_params;

// Code.
void destroy();
void initialize();
ssize_t pull_helper(gnutls_transport_ptr_t ptr, void* data, size_t size);
ssize_t push_helper(gnutls_transport_ptr_t ptr, void const* data, size_t size);
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_INTERNAL_HH
