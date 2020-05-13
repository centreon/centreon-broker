# /usr/bin/python3
"""
** Copyright 2019-2020 Centreon
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
"""

import re

sql_file = "../../simu/docker/installBroker.sql"
header_file = "../inc/com/centreon/broker/storage/table_max_size.hh"

dico = {}

pattern_ct = re.compile('CREATE TABLE `(.*)`')
column_ct = re.compile('\s*`(.*)` (varchar\(([0-9]*)\)|text)')
end_ct = re.compile('^\)')

with open(sql_file) as fp:
    line = fp.readline()
    in_block = False

    while line:
        if not in_block:
            m = pattern_ct.match(line)
            if m:
                print("New table {}".format(m.group(1)))
                current_table = m.group(1)
                in_block = True
        else:
            if end_ct.match(line):
                in_block = False
            else:
                m = column_ct.match(line)
                if m:
                    if m.group(3):
                        print("New text column {} of size {}".format(m.group(1), m.group(3)))
                        dico.setdefault(current_table, []).append((m.group(1), m.group(3)))
                    else:
                        print("New text column {} of 65534".format(m.group(1), m.group(2)))
                        dico.setdefault(current_table, []).append((m.group(1), m.group(2)))

        line = fp.readline()

    fp.close()




enum = """namespace storage {
enum storage_tables {
"""

cols = ""

for t,content in dico.items():
    enum += "  {},\n".format(t)

    cols += "enum {}_cols {{\n".format(t)
    sizes = "constexpr static uint32_t {}_size[] {{\n".format(t)
    for c in content:
        cols += "  {}_{},\n".format(t, c[0])
        sizes += "  {},\n".format(c[1])
    cols += "};\n"
    sizes += "};\n\n"
    cols += sizes

enum += "};\n"

with open(header_file, 'w') as fp:
    fp.write("""/*
** Copyright 2020 Centreon
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

#ifndef __TABLE_MAX_SIZE_HH__
#define __TABLE_MAX_SIZE_HH__

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

""")

    fp.write(enum)
    fp.write(cols)
    fp.write("\n}\n\n#CCB_END()\n\n#endif /* __TABLE_MAX_SIZE_HH__ */")
