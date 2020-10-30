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
import struct
from enum import Enum


class Type(Enum):
    BOOL = 1
    SHORT = 2
    STR = 3
    DOUBLE = 4
    INT32 = 5
    INT64 = 6
    TIMESTAMP = 7


events = {
    "1": {
        "1": {
            "name": "Acknowledgement"
        },
        "2": {
            "name": "Comment"
        },
        "3": {
            "name": "Custom variable"
        },
        "4": {
            "name": "Custom variable status"
        },
        "5": {
            "name": "Downtime"
        },
        "6": {
            "name": "Event handler"
        },
        "7": {
            "name": "Flapping status"
        },
        "8": {
            "name": "Host check"
        },
        "9": {
            "name": "Host dependency"
        },
        "10": {
            "name": "Host group"
        },
        "11": {
            "name": "Host group member"
        },
        "12": {
            "name": "Host",
            "data": [
                (Type.BOOL, "acknowledged"),
                (Type.SHORT, "acknowledgement_type"),
                (Type.STR, "action_url"),
                (Type.BOOL, "active_checks_enabled"),
                (Type.STR, "address"),
                (Type.STR, "alias"),
                (Type.STR, "check_freshness"),
                (Type.DOUBLE, "check_interval"),
                (Type.STR, "check_period"),
                (Type.SHORT, "check_type"),
                (Type.SHORT, "current_check_attempt"),
                (Type.SHORT, "current_state"),
                (Type.BOOL, "default_active_checks_enabled"),
                (Type.BOOL, "default_event_handler_enabled"),
                (Type.BOOL, "default_flap_detection_enabled"),
                (Type.BOOL, "default_notifications_enabled"),
                (Type.BOOL, "default_passive_checks_enabled"),
                (Type.SHORT, "downtime_depth"),
                (Type.STR, "display_name"),
                (Type.BOOL, "enabled"),
                (Type.STR, "event_handler"),
                (Type.BOOL, "event_handler_enabled"),
                (Type.DOUBLE, "execution_time"),
                (Type.DOUBLE, "first_notification_delay"),
                (Type.BOOL, "flap_detection_enabled"),
                (Type.BOOL, "flap_detection_on_down"),
                (Type.BOOL, "flap_detection_on_unreachable"),
                (Type.BOOL, "flap_detection_on_up"),
                (Type.DOUBLE, "freshness_threshold"),
                (Type.BOOL, "has_been_checked"),
                (Type.DOUBLE, "high_flap_threshold"),
                (Type.STR, "host_name"),
                (Type.INT32, "host_id"),
                (Type.STR, "icon_image"),
                (Type.STR, "icon_image_alt"),
                (Type.INT32, "poller_id"),
                (Type.BOOL, "is_flapping"),
                (Type.TIMESTAMP, "last_check"),
                (Type.SHORT, "last_hard_state"),
                (Type.TIMESTAMP, "last_hard_state_change"),
                (Type.TIMESTAMP, "last_notification"),
                (Type.TIMESTAMP, "last_state_change"),
                (Type.TIMESTAMP, "last_time_down"),
                (Type.TIMESTAMP, "last_time_unreachable"),
                (Type.TIMESTAMP, "last_time_up"),
                (Type.TIMESTAMP, "last_update"),
                (Type.DOUBLE, "latency"),
                (Type.DOUBLE, "low_flap_threshold"),
                (Type.SHORT, "max_check_attempts"),
                (Type.TIMESTAMP, "next_check"),
                (Type.TIMESTAMP, "next_notification"),
                (Type.BOOL, "no_more_notifications"),
                (Type.STR, "notes"),
                (Type.STR, "notes_url"),
                (Type.DOUBLE, "notification_interval"),
                (Type.SHORT, "notification_number"),
                (Type.STR, "notification_period"),
                (Type.BOOL, "notifications_enabled"),
                (Type.BOOL, "notify_on_down"),
                (Type.BOOL, "notify_on_downtime"),
                (Type.BOOL, "notify_on_flapping"),
                (Type.BOOL, "notify_on_recovery"),
                (Type.BOOL, "notify_on_unreachable"),
                (Type.BOOL, "obsess_over"),
                (Type.BOOL, "passive_checks_enabled"),
                (Type.DOUBLE, "percent_state_change"),
                (Type.DOUBLE, "retry_interval"),
                (Type.BOOL, "should_be_scheduled"),
                (Type.BOOL, "stalk_on_down"),
                (Type.BOOL, "stalk_on_unreachable"),
                (Type.BOOL, "stalk_on_up"),
                (Type.STR, "statusmap_image"),
                (Type.SHORT, "state_type"),
                (Type.STR, "check_command"),
                (Type.STR, "output"),
                (Type.STR, "perf_data"),
                (Type.BOOL, "retain_nonstatus_information"),
                (Type.BOOL, "retain_status_information"),
                (Type.STR, "timezone"),
            ]
        },
        "13": {
            "name": "Host parent"
        },
        "14": {
            "name": "Host status"
        },
        "15": {
            "name": "Instance"
        },
        "16": {
            "name": "Instance status"
        },
        "17": {
            "name": "Log entry",
            "data": [
                
            ]
        },
        "18": {
            "name": "Module"
        },
        "19": {
            "name": "Service check"
        },
        "20": {
            "name": "Service dependency"
        },
        "21": {
            "name": "Service group"
        },
        "22": {
            "name": "Service group member"
        },
        "23": {
            "name": "Service",
            "data": [
                (Type.BOOL, "acknowledged"),
                (Type.SHORT, "acknowledgement_type"),
                (Type.STR, "action_url"),
                (Type.BOOL, "active_checks"),
                (Type.BOOL, "check_freshness"),
                (Type.DOUBLE, "check_interval"),
                (Type.STR, "check_period"),
                (Type.SHORT, "check_type"),
                (Type.SHORT, "current_check_attempt"),
                (Type.SHORT, "current_state"),
                (Type.BOOL, "default_active_checks_enabled"),
                (Type.BOOL, "default_event_handler_enabled"),
                (Type.BOOL, "default_flap_detection_enabled"),
                (Type.BOOL, "default_notification_enabled"),
                (Type.BOOL, "default_passive_checks_enabled"),
                (Type.SHORT, "downtime_depth"),
                (Type.STR, "display_name"),
                (Type.BOOL, "enabled"),
                (Type.STR, "event_handler"),
                (Type.BOOL, "event_handler_enabled"),
                (Type.DOUBLE, "execution_time"),
                (Type.DOUBLE, "first_notification_delay"),
                (Type.BOOL, "flap_detection_enabled"),
                (Type.BOOL, "flap_detection_on_critical"),
                (Type.BOOL, "flap_detection_on_ok"),
                (Type.BOOL, "flap_detection_on_unknown"),
                (Type.BOOL, "flap_detection_on_warning"),
                (Type.DOUBLE, "freshness_threshold"),
                (Type.BOOL, "has_been_checked"),
                (Type.DOUBLE, "high_flap_threshold"),
                (Type.INT32, "host_id"),
                (Type.STR, "host_name"),
                (Type.STR, "icon_image"),
                (Type.STR, "icon_image_alt"),
                (Type.INT32, "service_id"),
                (Type.BOOL, "is_flapping"),
                (Type.BOOL, "is_volatile"),
                (Type.TIMESTAMP, "last_check"),
                (Type.SHORT, "last_hard_state"),
                (Type.TIMESTAMP, "last_hard_state_change"),
                (Type.TIMESTAMP, "last_notification"),
                (Type.TIMESTAMP, "last_state_change"),
                (Type.TIMESTAMP, "last_time_critical"),
                (Type.TIMESTAMP, "last_time_ok"),
                (Type.TIMESTAMP, "last_time_unknown"),
                (Type.TIMESTAMP, "last_time_warning"),
                (Type.TIMESTAMP, "last_update"),
                (Type.DOUBLE, "latency"),
                (Type.DOUBLE, "low_flap_threshold"),
                (Type.SHORT, "max_check_attempts"),
                (Type.TIMESTAMP, "next_check"),
                (Type.TIMESTAMP, "next_notification"),
                (Type.BOOL, "no_more_notifications"),
                (Type.STR, "notes"),
                (Type.STR, "note_url"),
                (Type.DOUBLE, "notification_interval"),
                (Type.SHORT, "notification_number"),
                (Type.STR, "notification_period"),
                (Type.BOOL, "notifications_enabled"),
                (Type.BOOL, "notify_on_critical"),
                (Type.BOOL, "notify_on_downtime"),
                (Type.BOOL, "notify_on_flapping"),
                (Type.BOOL, "notify_on_recovery"),
                (Type.BOOL, "notify_on_unknown"),
                (Type.BOOL, "notify_on_warning"),
                (Type.BOOL, "obsess_over"),
                (Type.BOOL, "passive_checks_enabled"),
                (Type.DOUBLE, "percent_state_change"),
                (Type.DOUBLE, "retry_interval"),
                (Type.STR, "service_description"),
                (Type.BOOL, "should_be_scheduled"),
                (Type.BOOL, "stalk_on_critical"),
                (Type.BOOL, "stalk_on_ok"),
                (Type.BOOL, "stalk_on_unknown"),
                (Type.BOOL, "stalk_on_warning"),
                (Type.SHORT, "state_type"),
                (Type.STR, "check_command"),
                (Type.STR, "output"),
                (Type.STR, "perf_data"),
                (Type.BOOL, "retain_nonstatus_information"),
                (Type.BOOL, "retain_status_information"),
            ]
        },
        "24": {
            "name": "Service status"
        },
        "25": {"name": "Instance configuration"}
    },
    '3': {
        "1": {"name": "metric"},
        "2": {"name": "rebuild"},
        "3": {"name": "remove_graph"},
        "4": {"name": "status"},
        "5": {"name": "index mapping"},
        "6": {"name": "metric mapping"}
    },
    "2": {"1": {"name": "version_response"},
          "2": {"name": "ack"}
          },
    "6": {"1": {"name": "ba_status"},
          "2": {"name": "kpi_status"},
          "3": {"name": "meta_service_status"},
          "4": {"name": "ba_event"},
          "5": {"name": "kpi_event"},
          "6": {"name": "ba_duration_event"},
          "7": {"name": "dimension_ba_event"},
          "8": {"name": "dimension_kpi_event"},
          "9": {"name": "dimension_ba_bv_relation_event"},
          "10": {"name": "dimension_bv_event"},
          "11": {"name": "dimension_truncate_table_signal"},
          "12": {"name": "rebuild"},
          "13": {"name": "dimension_timeperiod"},
          "14": {"name": "dimension_ba_timeperiod_relation"},
          "15": {"name": "dimension_timeperiod_exception"},
          "16": {"name": "dimension_timeperiod_exclusion"},
          "17": {"name": "inherited_downtime"}
          },
    "65535": {
        "1": {"name": "Internal 1"},
        "2": {"name": "Internal 2"}
    },
}


class Bbdo:
    def __init__(self, name: str):
        self.data = open(name, "rb").read()
        self.offset = 8

    def __init__(self, data: bytearray):
        self.data = data
        self.offset = 0

    def get_bool(self):
        (retval,) = struct.unpack_from('b', self.data, self.offset)
        self.offset += 1
        if retval == 1:
            return True
        elif retval == 0:
            return False
        else:
            assert retval == 0 or retval == 1

    def get_int32(self):
        (retval,) = struct.unpack_from('!i', self.data, self.offset)
        self.offset += 4
        return retval

    def get_int64(self):
        (retval,) = struct.unpack_from('!q', self.data, self.offset)
        self.offset += 8
        return retval

    def get_short(self):
        (retval,) = struct.unpack_from('!h', self.data, self.offset)
        self.offset += 2
        return retval

    def get_string(self):
        i = self.offset
        while self.data[i] != 0:
            i += 1
        retval = self.data[self.offset:i].decode("utf-8")
        self.offset = i + 1
        return retval

    def unserialize(self):
        (chksum, size, category, element, source_id, dest_id) = struct.unpack_from('!HHHHII', self.data, self.offset)
        self.offset += 16
        retval = "event: {}\n  size: {}".format(events[str(category)][str(element)]["name"], size)
        if 'data' in events[str(category)][str(element)]:
            for d in events[str(category)][str(element)]["data"]:
                if d[0] == Type.BOOL:
                    retval += "  {} = {}\n".format(d[1], self.get_bool())
                elif d[0] == Type.SHORT:
                    retval += "  {} = {}\n".format(d[1], self.get_short())
                elif d[0] == Type.STR or d[0] == Type.DOUBLE:
                    retval += "  {} = {}\n".format(d[1], self.get_string())
                elif d[0] == Type.INT32:
                    retval += "  {} = {}\n".format(d[1], self.get_int32())
                elif d[0] == Type.INT64 or d[0] == Type.TIMESTAMP:
                    retval += "  {} = {}\n".format(d[1], self.get_int64())

        return retval
