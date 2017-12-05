/*
** Copyright 2011-2013,2015,2017 Centreon
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

#ifndef CCB_COMPRESSION_ZLIB_HH
#  define CCB_COMPRESSION_ZLIB_HH

#  include <QByteArray>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             compression {
  /**
   *  @class zlib zlib.hh "com/centreon/broker/compression/zlib.hh"
   *  @brief Binding around the zlib library.
   *
   *  Compress and uncompress data.
   */
  class               zlib {
   public:
    static QByteArray compress(QByteArray const& data, int compression_level);
    static QByteArray uncompress(unsigned char const* data, unsigned long nbytes);
  };
}

CCB_END()

#endif // !CCB_COMPRESSION_ZLIB_HH
