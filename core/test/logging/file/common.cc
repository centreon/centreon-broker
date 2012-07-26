/*
** Copyright 2011-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
bool check_content(QString const& path,
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
    retval &= (line == "Centreon Broker log file opened\n");

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
    retval &= (line == "Centreon Broker log file closed\n");
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
