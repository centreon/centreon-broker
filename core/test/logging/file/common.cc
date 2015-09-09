/*
** Copyright 2011-2013 Centreon
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

#include <QRegExp>
#include <QString>
#include <QTemporaryFile>
#include "test/logging/file/common.hh"

using namespace com::centreon::broker;

/**
 *  Check a file content.
 *
 *  @param[in] path    File path.
 *  @param[in] pattern Core content pattern.
 *  @param[in] msg_nb  Number of messages to check.
 *  @param[in] lines   Lines if not all messages should be matched.
 *
 *  @return true if all log file match pattern.
 */
bool check_content(
       QString const& path,
       QString const& pattern,
       unsigned int msg_nb,
       char const* const* lines) {
  // Return value.
  bool retval(true);

  // Default content strings.
  static char const* const content[] =
    { MSG1, MSG2, MSG3, MSG4, MSG5, MSG6, MSG7, MSG8 };
  if (!lines)
    lines = content;

  // Open log file.
  QFile f(path);
  if (f.open(QIODevice::ReadOnly)) {
    // First line.
    f.waitForReadyRead(-1);
    QByteArray line(f.readLine());
    retval &= (line == "Centreon Broker " CENTREON_BROKER_VERSION " log file opened\n");

    // Match lines.
    for (unsigned int i = 0;
         (i < msg_nb) && retval;
         ++i) {
      // Read line.
      f.waitForReadyRead(-1);
      line = f.readLine();
      if (!line.isEmpty())
        line.resize(line.size() - 1);

      // Match line.
      QString my_pattern(pattern);
      if (!content[i])
        retval = false;
      else {
        my_pattern.replace("<MSG>", lines[i]);
        retval &= (QRegExp(my_pattern).indexIn(line) != -1);
      }
    }

    // Last line.
    f.waitForReadyRead(-1);
    line = f.readLine();
    retval &= (line == "Centreon Broker " CENTREON_BROKER_VERSION " log file closed\n");
    f.waitForReadyRead(-1);
    f.readLine();
    retval &= (!line.isEmpty() && f.atEnd());
  }
  else
    retval = false;

  // Return check result.
  return (retval);
}

/**
 *  Get a temporary file path.
 *
 *  @return Temporary file path.
 */
QString temp_file_path() {
  QTemporaryFile f;
  f.setAutoRemove(false);
  f.open();
  QString file_path(f.fileName());
  f.close();
  return (file_path);
}

/**
 *  Write log messages to a backend.
 *
 *  @param[out] b      Backend to write to.
 *  @param[in]  msg_nb Number of messages to write.
 */
void write_log_messages(logging::backend* b, unsigned int msg_nb) {
  // First message.
  b->log_msg(
    MSG1 "\n",
    sizeof(MSG1),
    logging::config_type,
    logging::high);
  if (msg_nb <= 1)
    return ;

  // Second message.
  b->log_msg(
    MSG2 "\n",
    sizeof(MSG2),
    logging::debug_type,
    logging::medium);
  if (2 == msg_nb)
    return ;

  // Third message.
  b->log_msg(
    MSG3 "\n",
    sizeof(MSG3),
    logging::error_type,
    logging::low);
  if (3 == msg_nb)
    return ;

  // Fourth message.
  b->log_msg(
    MSG4 "\n",
    sizeof(MSG4),
    logging::info_type,
    logging::high);
  if (4 == msg_nb)
    return ;

  // Fifth message.
  b->log_msg(
    MSG5 "\n",
    sizeof(MSG5),
    logging::config_type,
    logging::low);
  if (5 == msg_nb)
    return ;

  // Sixth message.
  b->log_msg(
    MSG6 "\n",
    sizeof(MSG6),
    logging::debug_type,
    logging::low);

  // Seventh message.
  b->log_msg(
    MSG7 "\n",
    sizeof(MSG7),
    logging::info_type,
    logging::medium);

  // Height message.
  b->log_msg(
    MSG8 "\n",
    sizeof(MSG8),
    logging::error_type,
    logging::medium);

  return ;
}
