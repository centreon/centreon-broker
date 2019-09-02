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
#include <thread>
#include <QByteArray>
#include <mutex>
#include <cstdlib>
#include <gtest/gtest.h>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/file/splitter.hh"
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/file/cfile.hh"
#include "com/centreon/broker/file/stl_fs_browser.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

#define BIG 50000
#define RETENTION_DIR "/tmp/"
#define RETENTION_FILE "test-concurrent-queue"

static std::mutex mutex;

class read_thread {
 public:
  read_thread(file::splitter* f, int size)
      : _file(f), _current(0), _buf(size, '\0'), _size(size) {
    _thread = std::thread(&read_thread::callback, this);
  }

  void join() {
    _thread.join();
  }
  QByteArray get_result() { return _buf; }

  void callback() {
    int ret = 0;

    do {
      try {
        std::lock_guard<std::mutex> lock(mutex);
        ret = _file->read(_buf.data() + _current, _size);
        _current += ret;
      } catch (...) {
      }
      usleep(100);
    } while (_current < _size);
  }

 private:
  std::thread _thread;
  file::splitter* _file;
  int _current;
  QByteArray _buf;
  int _size;
};

class write_thread {
 public:
  write_thread(file::splitter* f, int size) : _file(f), _size(size) {
    _thread = std::thread(&write_thread::callback, this);
  }

  void join() {
    _thread.join();
  }

  void callback() {
    char* buf = new char[_size];
    for (int i(0); i < _size; ++i)
      buf[i] = i & 255;

    int wb = 0;
    for (int j(0); j < _size; j += wb) {
      std::unique_lock<std::mutex> lock(mutex);
      wb = _file->write(buf + j, 100);
      lock.unlock();
      usleep(rand() % 100);
    }

    delete[] buf;
  }

 private:
  std::thread _thread;
  file::splitter* _file;
  int _size;
};

class FileSplitterConcurrent : public ::testing::Test {

 public:
  void SetUp() {
    _path = RETENTION_DIR RETENTION_FILE;
    _remove_files();
    _file_factory.reset(new cfile_factory());
    _fs_browser.reset(new stl_fs_browser());

    logging::manager::load();

    _file.reset(new file::splitter(
                            _path,
                            file::fs_file::open_read_write_truncate,
                            _file_factory.release(),
                            _fs_browser.release(),
                            10000,
                            true));
  }
  void TearDown() {
    logging::manager::unload();
  }

 protected:
  std::unique_ptr<file::splitter> _file;
  std::unique_ptr<cfile_factory>  _file_factory;
  std::unique_ptr<stl_fs_browser>  _fs_browser;
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

  wt.join();
  rt.join();

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
// And when we call the remove_all_files() method
// Then all the created files are removed.
TEST_F(FileSplitterConcurrent, MultipleFilesCreated) {
  write_thread wt(_file.get(), BIG);
  read_thread rt(_file.get(), BIG);

  rt.join();
  wt.join();

  QByteArray result(rt.get_result());
  QByteArray buffer(BIG, '\0');
  for (int i(0); i < BIG; ++i)
    buffer[i] = i & 255;

  // Then
  ASSERT_EQ(buffer.size(), result.size());
  ASSERT_EQ(buffer, result);

  // Then
  _file->remove_all_files();
  QDir dir(RETENTION_DIR);
  QStringList filters_list;
  filters_list << RETENTION_FILE;
  QStringList entries(dir.entryList(filters_list));
  ASSERT_EQ(entries.size(), 0);
}
