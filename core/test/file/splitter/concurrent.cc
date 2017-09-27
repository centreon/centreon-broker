/*
** Copyright 2017 Centreon
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

#include <QDir>
#include <QThread>
#include <QByteArray>
#include <QMutexLocker>
#include <cstdlib>
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/file/qt_fs_browser.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

#define BIG 50000
#define RETENTION "/tmp/test-concurrent-queue"

static QMutex mutex;

class read_thread: public QThread
{
 public:
  read_thread(file::splitter* f, int size)
    : _file(f), _current(0), _buf(size, '\0'), _size(size) {}

  QByteArray get_result() {
    return _buf;
  }

 private:
  file::splitter* _file;
  int _current;
  QByteArray _buf;
  int _size;

  void run() {
    int ret = 0;

    do {
      try {
        QMutexLocker lock(&mutex);
        ret = _file->read(_buf.data() + _current, _size);
        _current += ret;
      }
      catch (...) {}
      usleep(100);
    }
    while (_current < _size);
  }
};

class write_thread: public QThread
{
 public:
  write_thread(file::splitter* f, int size)
    : _file(f), _size(size) {}

 private:
  file::splitter* _file;
  int             _size;

  void run() {
    char* buf = new char[_size];
    for (int i(0); i < _size; ++i)
      buf[i] = i & 255;

    for (int j(0); j < _size; j += 100) {
      QMutexLocker lock(&mutex);
      int wb(_file->write(buf + j, 100));
      usleep(rand() % 100);
    }

    delete[] buf;
  }
};

class FileSplitterConcurrent : public ::testing::Test {

 public:
  void SetUp() {
    _path = RETENTION;
    _remove_files();
    _file_factory.reset(new cfile_factory());
    _fs_browser.reset(new qt_fs_browser());

    _file.reset(new file::splitter(
                            _path,
                            file::fs_file::open_read_write_truncate,
                            _file_factory.release(),
                            _fs_browser.release(),
                            10000,
                            true));
    return ;
  }

 protected:
  std::auto_ptr<file::splitter> _file;
  std::auto_ptr<cfile_factory>  _file_factory;
  std::auto_ptr<qt_fs_browser>  _fs_browser;
  std::string                   _path;

  void _remove_files() {
    QString dirname(_path.c_str());
    int idx(dirname.lastIndexOf("/"));
    QString path = dirname.mid(idx + 1) + "*";
    dirname.resize(idx + 1);
    QDir dir(dirname);
    QStringList filters_list;
    filters_list << path;
    QStringList entries(dir.entryList(filters_list));
    for (QStringList::iterator it(entries.begin()), end(entries.end());
      it != end; ++it) {
      QFile::remove(dirname + *it);
    }
  }

};

// Given a splitter object
// When we write and read at the same time the object
// Then the read buffer contains the same content than the written buffer.
TEST_F(FileSplitterConcurrent, DefaultFile) {
  write_thread wt(_file.get(), 1000);
  read_thread rt(_file.get(), 1000);

  wt.start();
  rt.start();

  rt.wait();
  wt.wait();

  QByteArray result(rt.get_result());
  QByteArray buffer(1000, '\0');
  for (int i(0); i < 1000; ++i)
    buffer[i] = i & 255;

  // Then
  ASSERT_EQ(buffer.size(), result.size());
  ASSERT_EQ(buffer, result);
}

// Given a splitter object
// When we write and read at the same time the object while data are too
// long to be store in a simple file
// Then the read buffer contains the same content than the written buffer.
TEST_F(FileSplitterConcurrent, MultipleFilesCreated) {
  write_thread wt(_file.get(), BIG);
  read_thread rt(_file.get(), BIG);

  wt.start();
  rt.start();

  rt.wait();
  wt.wait();

  QByteArray result(rt.get_result());
  QByteArray buffer(BIG, '\0');
  for (int i(0); i < BIG; ++i)
    buffer[i] = i & 255;

  // Then
  ASSERT_EQ(buffer.size(), result.size());
  ASSERT_EQ(buffer, result);
}

