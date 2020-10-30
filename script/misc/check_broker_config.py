##
## Copyright 2020 Centreon
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## For more information : contact@centreon.com
##
import json
import os
import glob

DIR_PATH    = "/var/cache/centreon/config/broker/"
ERROR_PATH  = "/tmp/conf-error.log"
data        = []
debug       = True

class Input:
    def __init__(self, json_input):
        self.__json      = json_input
        self.__poller_id = 0
        self.__broker_id = 0
        self.__output_matched = []
        self.__msg_log = "undef"
    def add_output(self, output):
        self.__output_matched.append(output)
    def add_poller_id(self, id):
        self.__poller_id = id
    def add_broker_id(self, id):
        self.__broker_id = id
    def add_msg_log(self, msg):
        self.__msg_log = msg
    def get_json(self):
        return self.__json
    def get_name(self):
        if "name" in self.__json:
            return self.__json["name"]
        else:
            return "undefined"
    def get_protocol(self):
        if "protocol" in self.__json:
            return self.__json["protocol"]
        else:
            return "undefined"
    def get_poller_id(self):
        return self.__poller_id
    def get_broker_id(self):
        return self.__broker_id
    def get_msg_log(self):
        return self.__msg_log
    def is_matched(self):
        return len(self.__output_matched) > 0
    def print_childs(self):
        for o in self.__output_matched:
           print(self.__json["name"] + " has " + o.get_name() + " for child ")

class Output:
    def __init__(self, json_output):
        self.__json      = json_output
        self.__poller_id = 0
        self.__broker_id = 0
        self.__input_matched = []
        self.__msg_log = "undef"
    def add_input(self, input):
        self.__input_matched.append(input)
    def add_poller_id(self, id):
        self.__poller_id = id
    def add_broker_id(self, id):
        self.__broker_id = id
    def add_msg_log(self, msg):
        self.__msg_log = msg
    def get_json(self):
        return self.__json
    def get_name(self):
        if "name" in self.__json:
            return self.__json["name"]
        else:
            return "undefined"
    def get_protocol(self):
        if "protocol" in self.__json:
            return self.__json["protocol"]
        else:
            return "undefined"
    def get_poller_id(self):
        return self.__poller_id
    def get_broker_id(self):
        return self.__broker_id
    def get_msg_log(self):
        return self.__msg_log
    def is_matched(self):
        return len(self.__input_matched) > 0
    def print_childs(self):
        for o in self.__input_matched:
           print(self.__json["name"] + " has " + o.get_name() + " for child ")

#read all json files
def read_json():
    nfolders  = 0
    for _, dirnames, _ in os.walk(DIR_PATH):
        nfolders += len(dirnames)
    for current_dir in range(1, nfolders + 1) :
        for filename in glob.glob(os.path.join(DIR_PATH + str(current_dir), '*.json')):
            with open(filename) as json_file:
                data.append(json.load(json_file))

#check if in our input items we have at least one bbdo protocol
def at_least_one_bbdo(protocols):
    for protocol in protocols:
        if protocol == "bbdo":
            return True
    return False

def print_poller_id():
    for i in range(0, len(data)):
        print("poller_id : ", data[i]["centreonBroker"]["poller_id"])

def remove_useless_json():
    i = 0
    #check if we have a poller_id item
    while i < len(data):
        if not "poller_id" in data[i]["centreonBroker"]:
            data.pop(i)
        i += 1

    if debug:
        print("=== remove useless json : first check ===")
        print_poller_id()

    #check if we have at least one input or ouput item
    i = 0
    while i < len(data):
        if not "output" in data[i]["centreonBroker"] and not "input" in data[i]["centreonBroker"]:
            data.pop(i)
        i += 1
    if debug:
        print("=== remove useless json : second check ===")
        print_poller_id()

    #check if in our inputs and ouputs items we have at least one bbdo protocol
    i = 0
    while i < len(data):
        protocols = []
        if "input" in data[i]["centreonBroker"]:
            for input_item in data[i]["centreonBroker"]["input"]:
                if "protocol" in input_item:
                    protocols.append(input_item["protocol"])
        if "output" in data[i]["centreonBroker"]:
            for output_item in data[i]["centreonBroker"]["output"]:
                if "protocol" in output_item:
                    protocols.append(output_item["protocol"])
        if debug:
            print("poller id ", data[i]["centreonBroker"]["poller_id"], protocols)
        if not at_least_one_bbdo(protocols):
           data.pop(i)
        else:
            i += 1

    if debug:
        print("=== remove useless json : third check ===")
        print_poller_id()

def send_error(f, item_name, msg):
    f.write(msg + " for " + item_name + "\n")

def analyze_input_output(input, output):
    if ((not "negotiation" in input or not "negotiation" in output)
        or (not "protocol" in input or not "protocol" in output)
        or (not "type" in input or not "type" in output)
        or (not "port" in input or not "port" in output)
        or (not "one_peer_retention_mode" in input or not "one_peer_retention_mode" in output)):
        return [False, "Error : missing fields"]
    if not "tls" in input:
        input["tls"] = "nothing"
    if not "tls" in output:
        output["tls"] = "nothing"
    if not "compression" in input:
        input["compression"] = "nothing"
    if not "compression" in output:
        output["compression"] = "nothing"
    if not input["protocol"] == "bbdo" or not output["protocol"] == "bbdo":
        return [False, "Error : no protocol bbdo"]
    if not input["type"] == "ipv4" or not output["type"] =="ipv4":
        return [False, "Error : type is not ipv4"]
    if input["port"] != output["port"]:
        return [False, "Error : no matching port"]
    if ((input["tls"] == "yes" and output["tls"] == "no") 
        or (input["tls"] == "no" and output["tls"] == "yes")):
        return [False, "Error : different tls value for input and output"]
    if ((input["negotiation"] == "no" and output["negotiation"] == "yes") 
        or (input["negotiation"] == "yes" and output["negotiation"] == "no")):
        return [False, "Error: different negociation value for input and output"]
    if input["negotiation"] == "no" and output["negotiation"] == "no":
        if (input["tls"] == "auto" 
            or input["compression"] == "auto" 
            or output["compression"] == "auto" 
            or output["tls"] == "auto"):
            return [False, "Warning : negociation is no but auto in tls or compression field"]
    if ((input["one_peer_retention_mode"] == "yes" 
        and output["one_peer_retention_mode"] == "no") 
        or (input["one_peer_retention_mode"] == "no" 
        and output["one_peer_retention_mode"] == "yes")):
        return [False, "Error : different one_peer_retention_mode value"]
    if (input["one_peer_retention_mode"] == "yes" 
        and output["one_peer_retention_mode"] == "yes"):
        if not "host" in input and "host" in output:
            return [False, "Error : one_peer_retention_mode is yes but host is in output"]
    if input["one_peer_retention_mode"] == "no" and output["one_peer_retention_mode"] == "no":
        if not "host" in output or "host" in input:
            return [False, "Error : one_peer_retention_mode is no but host is in input"]
    return [True, "no error"]

# here we check if an input of a configuration file match with an output in another configuration file
def match_input_output():
    input_lst  = []
    output_lst = []

    for i in range(0, len(data)):
        if "input" in data[i]["centreonBroker"]:
            for input_item in data[i]["centreonBroker"]["input"]:
                input_tmp = Input(input_item)
                input_tmp.add_poller_id(data[i]["centreonBroker"]["poller_id"])
                input_tmp.add_broker_id(data[i]["centreonBroker"]["broker_id"])
                input_lst.append(input_tmp)
    for i in range(0, len(data)):
        if "output" in data[i]["centreonBroker"]:
            for output_item in data[i]["centreonBroker"]["output"]:
                output_tmp = Output(output_item)
                output_tmp.add_poller_id(data[i]["centreonBroker"]["poller_id"])
                output_tmp.add_broker_id(data[i]["centreonBroker"]["broker_id"])
                output_lst.append(output_tmp)
    if debug:
        for i in input_lst: 
            print(i.get_name(), i.get_broker_id())
        for o in output_lst: 
            print(o.get_name(), o.get_broker_id())

    for input_item in input_lst:
        for output_item in output_lst:
            if input_item.get_broker_id() != output_item.get_broker_id():
                ret = analyze_input_output(input_item.get_json(), 
                    output_item.get_json())
                if ret[0]:
                    input_item.add_output(output_item)
                    output_item.add_input(input_item)
                input_item.add_msg_log(ret[1])
                output_item.add_msg_log(ret[1])

    return [input_lst, output_lst]

# this function checks if a bbdo protocol has not been matched
def check_input_output(input_output_lst):
    f = open(ERROR_PATH, 'a')

    for i in input_output_lst[0]: 
        if i.get_protocol() == "bbdo" and not i.is_matched():
            send_error(f, i.get_name(), i.get_msg_log())
    for o in input_output_lst[1]: 
        if o.get_protocol() == "bbdo" and not o.is_matched():
            send_error(f, o.get_name(), o.get_msg_log()) 

    f.close()

if __name__ == "__main__":
    read_json()
    remove_useless_json()
    io = match_input_output()
    check_input_output(io)

