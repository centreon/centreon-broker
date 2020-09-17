#!/usr/bin/python3
"""
** Copyright 2019-2020 Centreon
**
** Licensed under the Apache License, Version 2.0(the "License");
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

import sys
import re

dico = {}
pattern_ct = re.compile('CREATE TABLE( IF NOT EXISTS)? (@.*@\.)?`?([^`]*)`? \(')
column_ct = re.compile('\s*`?([^`]*)`? (varchar\(([0-9]*)\)|text)')
end_ct = re.compile('^\)')

debug = False

def print_dbg(s):
    if debug:
        print(s)

header_file = sys.argv[1]
for sql_file in sys.argv[2:]:
    with open(sql_file, encoding="utf-8") as fp:
        line = fp.readline()
        in_block = False

        while line:
            if not in_block:
                m = pattern_ct.match(line)
                if m:
                    print_dbg("New table {}".format(m.group(3)))
                    current_table = m.group(3)
                    in_block = True
            else:
                if end_ct.match(line):
                    in_block = False
                else:
                    m = column_ct.match(line)
                    if m:
                        if m.group(3):
                            print_dbg("New text column {} of size {}".format(m.group(1), m.group(3)))
                            dico.setdefault(current_table, []).append((m.group(1), m.group(3)))
                        else:
                            print_dbg("New text column {} of 65534".format(m.group(1), m.group(2)))
                            dico.setdefault(current_table, []).append((m.group(1), 65534))

            line = fp.readline()

        fp.close()

cols = ""

for t,content in dico.items():
    cols += "enum {}_cols {{\n".format(t)
    sizes = "constexpr static uint32_t {}_size[] {{\n".format(t)
    for c in content:
        cols += "  {}_{},\n".format(t, c[0])
        sizes += "    {},\n".format(c[1])
    cols += "};\n"
    sizes += "};\n"
    cols += sizes

    cols += """constexpr uint32_t get_{}_col_size(
    {}_cols const& col) {{
  return {}_size[col];
}}

""".format(t, t, t)

with open(header_file, 'w', encoding="utf-8") as fp:
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

    #fp.write(enum)
    fp.write(cols)
    fp.write("\n\nCCB_END()\n\n#endif /* __TABLE_MAX_SIZE_HH__ */")
