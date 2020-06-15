.. _dev_mapping:

#############################
Centreon Broker Event Mapping
#############################

Centreon Broker uses global mapping tables for events that can be
exchanged. How exactly these mapping tables work is left to discover to
the reader. This page list properties available for each event type.

***
NEB
***

Acknowledgement
===============

======================================== ================ ====================================================== =============
Property                                 Type             Description                                            Version
======================================== ================ ====================================================== =============
acknowledgement_type                     short integer    Host acknowledgement when 0,
                                                          service acknowledgement when 1.
author                                   string           Acknowledgement author.
comment                                  string           Comment associated to the
                                                          acknowledgement.
deletion_time                            time             Time at which the acknowledgement
                                                          was deleted. If 0, it was not
                                                          deleted.
entry_time                               time             Time at which the acknowledgement
                                                          was created.
host_id                                  unsigned integer Host ID.
instance_id                              unsigned integer Instance ID.
is_sticky                                boolean          Sticky flag.
notify_contacts                          boolean          Notification flag.
persistent_comment                       boolean          True if the comment is persistent.
service_id                               unsigned integer Service ID. 0 for a host
                                                          acknowledgement.
state                                    short integer    Host / service state.
notify_only_if_not_already_acknowledged  boolean          A notification should be sent only if not already ack.
======================================== ================ ====================================================== =============

Comment
=======

============== ================ ========================================= =============
Property       Type             Description                               Version
============== ================ ========================================= =============
author         string           Comment author.
comment_type   short integer    1 for a host comment, 2 for a service
                                comment.
data           string           Comment data (text).
deletion_time  time             Time at which the comment was deleted. 0
                                if the comment was not deleted (yet).
entry_time     time             Time at which the comment was created.
entry_type     short integer    1 for a user comment (through external
                                command), 2 for a downtime comment, 3 for
                                a flapping comment and 4 for an
                                acknowledgement comment.
expire_time    time             Comment expiration time. 0 if no
                                expiration time.
expires        bool             True if the comment expires.
host_id        unsigned integer Host ID.
internal_id    unsigned integer Internal monitoring engine ID of the
                                comment.
persistent     boolean          True if the comment is persistent.
instance_id    unsigned integer Instance ID.
service_id     unsigned integer Service ID. 0 if this is a host comment.
source         short integer    0 when the comment originates from the
                                monitoring engine (internal) or 1 when
                                the comment comes from another source
                                (external).
============== ================ ========================================= =============

Custom variable
===============

============== ================ =========================================== =============
Property       Type             Description                                 Version
============== ================ =========================================== =============
enabled        boolean          True if the custom variable is enabled.
host_id        unsigned integer Host ID.
modified       boolean          True if the variable was modified.
name           string           Variable name.
service_id     unsigned integer Service ID. 0 if this is a host custom
                                variable.
update_time    time             Last time at which the variable was
                                updated.
var_type       short integer    0 for a host custom variable, 1 for a
                                service custom variable.
value          string           Variable value.
default_value  string           The default value of the custom var.
============== ================ =========================================== =============

Custom variable status
======================

Custom variable status events are generated when a custom variable needs
to be updated.

============== ================ =========================================== =============
Property       Type             Description                                 Version
============== ================ =========================================== =============
host_id        unsigned integer Host ID.
modified       boolean          True if the variable was modified.
name           string           Variable name.
service_id     unsigned integer Service ID. 0 if this is a host custom
                                variable.
update_time    time             Last time at which the variable was
                                updated.
value          string           Variable value.
============== ================ =========================================== =============

Downtime
========

================= ================ ===================================== =================
Property          Type             Description                           Version
================= ================ ===================================== =================
actual_end_time   time             Actual time at which the downtime
                                   ended.
actual_start_time time             Actual time at which the downtime
                                   started.
author            string           Downtime creator.
downtime_type     short integer    1 for a service downtime, 2 for a
                                   host downtime.
deletion_time     time             Time at which the downtime was
                                   deleted.
duration          time             Downtime duration.
end_time          time             Scheduled downtime end time.
entry_time        time             Time at which the downtime was
                                   created.
fixed             boolean          True if the downtime is fixed, false
                                   if it is flexible.
host_id           unsigned integer Host ID.
instance_id       unsigned integer Instance ID.
internal_id       unsigned integer Internal monitoring engine ID.
service_id        unsigned integer Service ID. 0 if this is a host
                                   downtime.
start_time        time             Scheduled downtime start time.
triggered_by      unsigned integer Internal ID of the downtime that
                                   triggered this downtime.
was_cancelled     boolean          True if the downtime was cancelled.
was_started       boolean          True if the downtime has been
                                   started.
comment           string           Downtime comment.
is_recurring      boolean          True if this downtime is recurring.   Only Broker
                                                                         managed downtimes
recurring_tp      string           The recurring timepriod of the        Only Broker
                                   recurring downtime.                   managed downtimes
come_from         short            Id of the parent recurring downtime   Only Broker
                                   for spawned downtimes.                managed downtimes
================= ================ ===================================== =================

Event handler
=============

============== ================ ======================================== =============
Property       Type             Description                              Version
============== ================ ======================================== =============
early_timeout  boolean          True if the event handler timed out.
end_time       time             Time at which the event handler
                                execution ended.
execution_time real             Execution time in seconds.
handler_type   short integer    0 for host-specific event handler, 1 for
                                service-specific event handler, 2 for
                                global host event handler and 3 for
                                global service event handler.
host_id        unsigned integer Host ID.
return_code    short integer    Value returned by the event handler.
service_id     unsigned integer Service ID. 0 if this is a host event
                                handler.
start_time     time             Time at which the event handler started.
state          short integer    Host / service state.
state_type     short integer    0 for SOFT, 1 for HARD.
timeout        short integer    Event handler timeout in seconds.
command_args   string           Event handler arguments.
command_line   string           Event handler command line.
output         string           Output returned by the event handler.
source_id      unsigned integer The id of the source instance of
                                this event.
destination_id unsigned integer The id of the destination
                                instance of this event.
============== ================ ======================================== =============

Flapping status
===============

==================== ================ ================================== =============
Property             Type             Description                        Version.
==================== ================ ================================== =============
event_time           time
event_type           integer
flapping_type        short integer
high_threshold       real             High flapping threshold.
host_id              unsigned integer Host ID.
low_threshold        real             Low flapping threshold.
percent_state_change real
reason_type          short integer
service_id           unsigned integer Service ID. 0 if this is a host
                                      flapping entry.
==================== ================ ================================== =============

Host
====

============================== ================ ========================= =============
Property                       Type             Description               Version
============================== ================ ========================= =============
acknowledged                   boolean
acknowledgement_type           short integer
action_url                     string
active_checks_enabled          boolean
address                        string
alias                          string
check_freshness                boolean
check_interval                 real
check_period                   string
check_type                     short integer
current_check_attempt          short integer
current_state                  short integer
default_active_checks_enabled  boolean
default_event_handler_enabled  boolean
default_flap_detection_enabled boolean
default_notifications_enabled  boolean
default_passive_checks_enabled boolean
downtime_depth                 short integer
display_name                   string
enabled                        boolean
event_handler                  string
event_handler_enabled          boolean
execution_time                 real
first_notification_delay       real
flap_detection_enabled         boolean
flap_detection_on_down         boolean
flap_detection_on_unreachable  boolean
flap_detection_on_up           boolean
freshness_threshold            real
has_been_checked               boolean
high_flap_threshold            real
host_name                      string
host_id                        unsigned integer
icon_image                     string
icon_image_alt                 string
instance_id                    unsigned integer
is_flapping                    boolean
last_check                     time
last_hard_state                short integer
last_hard_state_change         time
last_notification              time
last_state_change              time
last_time_down                 time
last_time_unreachable          time
last_time_up                   time
last_update                    time
latency                        real
low_flap_threshold             real
max_check_attempts             short integer
next_check                     time
next_notification              time
no_more_notifications          boolean
notes                          string
notes_url                      string
notification_interval          real
notification_number            short integer
notification_period            string
notifications_enabled          boolean
notify_on_down                 boolean
notify_on_downtime             boolean
notify_on_flapping             boolean
notify_on_recovery             boolean
notify_on_unreachable          boolean
obsess_over                    boolean
passive_checks_enabled         boolean
percent_state_change           real
retry_interval                 real
should_be_scheduled            boolean
stalk_on_down                  boolean
stalk_on_unreachable           boolean
stalk_on_up                    boolean
statusmap_image                string
state_type                     short integer
check_command                  string
output                         string
perf_data                      string
retain_nonstatus_information   boolean
retain_status_information      boolean
timezone                       string
============================== ================ ========================= =============

Host check
==========

===================== ================ ================================= =============
Property              Type             Description                       Version
===================== ================ ================================= =============
active_checks_enabled boolean          True if active checks are enabled
                                       on the host.
check_type            short integer
host_id               unsigned integer Host ID.
next_check            time             Time at which the next check is
                                       scheduled.
command_line          string           Check command line.
source_id             unsigned integer The id of the source
                                       instance this event.
destination_id        unsigned integer The id of the destination
                                       instance of this event.
===================== ================ ================================= =============

Host dependency
===============

============================ ================ ========================== =============
Property                     Type             Description                Version
============================ ================ ========================== =============
dependency_period            string
dependent_host_id            unsigned integer
enabled                      boolean
execution_failure_options    string
inherits_parent              boolean
host_id                      unsigned integer
notification_failure_options string
============================ ================ ========================== =============

Host group
==========

============== ================ =========================================== =============
Property       Type             Description                                 Version
============== ================ =========================================== =============
host_group_id  unsigned integer
name           string           Group name.
enabled        boolean          True if the group is enabled, false if it
                                is not (deletion).
poller_id      unsigned integer
============== ================ =========================================== =============

Host group member
=================

============== ================ =========================================== =============
Property       Type             Description                                 Version
============== ================ =========================================== =============
enabled        boolean          True if the membership is enabled, false if
                                it is not (deletion).
group          string           Group name.
instance_id    unsigned integer Instance ID.
host_id        unsigned integer Host ID.
source_id      unsigned integer The id of the source instance this event.
destination_id unsigned integer The id of the destination instance of this
                                event.
============== ================ =========================================== =============

Host parent
===========

============== ================ ============================================= =============
Property       Type             Description                                   Version
============== ================ ============================================= =============
enabled        boolean          True if parenting is enabled, false if it is
                                not (deletion).
child_id       unsigned integer Child host ID.
parent_id      unsigned integer Parent host ID.
============== ================ ============================================= =============

Host status
===========

============================= ================ ========================= =============
Property                      Type             Description               Version
============================= ================ ========================= =============
acknowledged                  boolean
acknowledgement_type          short integer
active_checks_enabled         boolean
check_interval                real
check_period                  string
check_type                    short integer
current_check_attempt         short integer
current_state                 short integer
downtime_depth                short integer
enabled                       boolean
event_handler                 string
event_handler_enabled         boolean
execution_time                real
flap_detection_enabled        boolean
has_been_checked              boolean
host_id                       unsigned integer
is_flapping                   boolean
last_check                    time
last_hard_state               short integer
last_hard_state_change        time
last_notification             time
last_state_change             time
last_time_down                time
last_time_unreachable         time
last_time_up                  time
last_update                   time
latency                       real
max_check_attempts            short integer
next_check                    time
next_host_notification        time
no_more_notifications         boolean
notification_number           short integer
notifications_enabled         boolean
obsess_over                   boolean
passive_checks_enabled        boolean
percent_state_change          real
retry_interval                real
should_be_scheduled           boolean
state_type                    short integer
check_command                 string
output                        string
perf_data                     string
============================= ================ ========================= =============

Instance
========

============== ================ ========================================= =============
Property       Type             Description                               Version
============== ================ ========================================= =============
engine         string           Name of the monitoring engine used on
                                this instance.
id             unsigned integer Instance ID.
name           string           Instance name.
is_running     boolean          Whether or not this instance is running.
pid            unsigned integer Monitoring engine PID.
program_end    time             Time at which the instance shut down.
program_start  time             Time at which the instance started.
version        string           Version of the monitoring engine used on
                                this instance.
============== ================ ========================================= =============

Instance status
===============

============================== ================ ================================ =============
Property                       Type             Description                      Version
============================== ================ ================================ =============
active_host_checks_enabled     boolean          Whether or not active
                                                host checks are globally
                                                enabled.
active_service_checks_enabled  boolean          Whether or not active
                                                service checks are
                                                globally enabled.
check_hosts_freshness          boolean          Whether or not hosts
                                                freshness checking is
                                                globally enabled.
check_services_freshness       boolean          Whether or not services
                                                freshness checking is
                                                globally enabled.
event_handler_enabled          boolean          Whether or not event
                                                handlers are globally
                                                enabled.
flap_detection_enabled         boolean          Whether or not flap
                                                detection is globally
                                                enabled.
id                             unsigned integer Instance ID.
last_alive                     time             Last time the instance
                                                was known alive.
last_command_check             time             Last time a check
                                                command was executed.
notifications_enabled          boolean          Whether or not
                                                notifications are
                                                globally enabled.
obsess_over_hosts              boolean          Whether or not the
                                                monitoring engine should
                                                obsess over hosts.
obsess_over_services           boolean          Whether or not the
                                                monitoring engine should
                                                obsess over services.
passive_host_checks_enabled    boolean          Whether or not passive
                                                host checks are globally
                                                enabled.
passive_service_checks_enabled boolean          Whether or not passive
                                                service checks are
                                                globally enabled.
global_host_event_handler      string           Global host event
                                                handler.
global_service_event_handler   string           Global service event
                                                handler.
============================== ================ ================================ =============

Log entry
=========

==================== ================ =================================== =============
Property             Type             Description                         Version
==================== ================ =================================== =============
c_time               time             Log time.
host_id              unsigned integer Host ID. 0 if log entry does not
                                      refer to a specific host or
                                      service.
host_name            string           Host name. Can be empty if log
                                      entry does not refer to a specific
                                      host or service.
instance_name        string           Instance name.
issue_start_time     time             Issue start time if correlation is
                                      enabled and log entry refers to an
                                      issue.
log_type             short integer    0 for SOFT, 1 for HARD.
msg_type             short integer    0 for SERVICE ALERT (sent on
                                      service state change), 1 for HOST
                                      ALERT (sent on host state change(,
                                      2 for SERVICE NOTIFICATION
                                      (notification sent out for a
                                      service), 3 for HOST NOTIFICATION
                                      (notification sent out for a host),
                                      4 for Warning (Centreon Engine
                                      warning), 5 for EXTERNAL COMMAND
                                      (external command received), 6 for
                                      CURRENT SERVICE STATE (current
                                      state of monitored service, usually
                                      sent at configuration reload), 7
                                      for CURRENT HOST STATE (current
                                      state of monitored host, usually
                                      sent at configuration reload), 8
                                      for INITIAL SERVICE STATE (initial
                                      state of service, after retention
                                      processing, sent at process start),
                                      9 for INITIAL HOST STATE (initial
                                      state of monitored host, after
                                      retention processing, sent at
                                      process start), 10 for
                                      ACKNOWLEDGE_SVC_PROBLEM external
                                      command (special case of EXTERNAL
                                      COMMAND for service
                                      acknowledgement), 11 for
                                      ACKNOWLEDGE_HOST_PROBLEM external
                                      command (special case of EXTERNAL
                                      COMMAND for host acknowledgement).
notification_cmd     string           Notification command.
notification_contact string           Notification contact.
retry                integer          Current check attempt.
service_description  string           Service description. Empty if log
                                      entry does not refer to a specific
                                      service.
service_id           unsigned integer Service ID. 0 if log entry does
                                      not refer to a specific service.
status               short integer    Host / service status.
output               string           Output.
==================== ================ =================================== =============

Module
======

Module events are generated when Centreon Broker modules get loaded or
unloaded.

================ ================ ====================================== =============
Property         Type             Description                            Version
================ ================ ====================================== =============
args             string           Module arguments.
enabled          boolean          Whether or not this module is enabled.
filename         string           Path to the module file.
instance_id      unsigned integer Instance ID.
loaded           boolean          Whether or not this module is loaded.
should_be_loaded boolean          Whether or not this module should be
                                  (should have been) loaded.
================ ================ ====================================== =============

Service
=======

============================== ================ ================================ =============
Property                       Type             Description                      Version
============================== ================ ================================ =============
acknowledged                   boolean
acknowledgement_type           short integer
action_url                     string
active_checks_enabled          boolean
check_freshness                boolean
check_interval                 real
check_period                   string
check_type                     short integer
current_check_attempt          short integer
current_state                  short integer
default_active_checks_enabled  boolean
default_event_handler_enabled  boolean
default_flap_detection_enabled boolean
default_notifications_enabled  boolean
default_passive_checks_enabled boolean
dowtine_depth                  short integer
display_name                   string
enabled                        boolean
event_handler                  string
event_handler_enabled          boolean
execution_time                 real
first_notification_delay       real
flap_detection_enabled         boolean
flap_detection_on_critical     boolean
flap_detection_on_ok           boolean
flap_detection_on_unknown      boolean
flap_detection_on_warning      boolean
freshness_threshold            real
has_been_checked               boolean
high_flap_threshold            real
host_id                        unsigned integer
host_name                      string
icon_image                     string
icon_image_alt                 string
service_id                     unsigned integer
is_flapping                    boolean
is_volatile                    boolean
last_check                     time
last_hard_state                short integer
last_hard_state_change         time
last_notification              time
last_state_change              time
last_time_critical             time
last_time_ok                   time
last_time_unknown              time
last_time_warning              time
last_update                    time
latency                        real
low_flap_threshold             real
max_check_attempts             short integer
next_check                     time
next_notification              time
no_more_notifications          boolean
notes                          string
notes_url                      string
notification_interval          real
notification_number            short integer
notification_period            string
notifications_enabled          boolean
notify_on_critical             boolean
notify_on_downtime             boolean
notify_on_flapping             boolean
notify_on_recovery             boolean
notify_on_unknown              boolean
notify_on_warning              boolean
obsess_over                    boolean
passive_checks_enabled         boolean
percent_state_change           real
retry_interval                 real
scheduled_downtime_depth       short integer
service_description            string
should_be_scheduled            boolean
stalk_on_critical              boolean
stalk_on_ok                    boolean
stalk_on_unknown               boolean
stalk_on_warning               boolean
state_type                     short integer
check_command                  string
output                         string
perf_data                      string
retain_nonstatus_information   boolean
retain_status_information      boolean
============================== ================ ================================ =============

Service check
=============

===================== ================ ================================= =============
Property              Type             Description                       Version
===================== ================ ================================= =============
active_checks_enabled boolean          True if active checks are enabled
                                       on the service.
check_type            short
host_id               unsigned integer Host ID.
next_check            time             Time at which the next check is
                                       scheduled.
service_id            unsigned integer Service ID.
command_line          string           Check command line.
===================== ================ ================================= =============

Service dependency
==================

============================ ================ ============================================== =============
Property                     Type             Description                                    Version
============================ ================ ============================================== =============
dependency_period            string
dependent_host_id            unsigned integer
dependent_service_id         unsigned integer
enabled                      boolean
execution_failure_options    string
host_id                      unsigned integer
inherits_parent              boolean
notification_failure_options string
service_id                   unsigned integer
============================ ================ ============================================== =============

Service group
=============

================ ================ ============================================ =============
Property         Type             Description                                  Version
================ ================ ============================================ =============
id               unsigned integer
name             string           Group name.
enabled          enabled          True if the group is enable, false if it is
                                  not (deletion).
poller_id        unsigned integer
================ ================ ============================================ =============

Service group member
====================

=============== ================ ============================================ =============
Property        Type             Description                                  Version
=============== ================ ============================================ =============
id              unsigned integer
host_id         unsigned integer
service_id      unsigned integer
enabled         enabled          True if the group is enable, false if it is
                                 not (deletion).
group_name      string           Group name.
poller_id       unsigned integer
=============== ================ ============================================ =============

Service status
==============

============================= ================ ============================================= =============
Property                      Type             Description                                   Version
============================= ================ ============================================= =============
acknowledged                  boolean
acknowledgement_type          short integer
active_checks_enabled         boolean
check_interval                real
check_period                  string
check_type                    short integer
current_check_attempt         short integer
current_state                 short integer
downtime_depth                short integer
enabled                       boolean
event_handler                 string
event_handler_enabled         boolean
execution_time                real
flap_detection_enabled        boolean
has_been_checked              boolean
host_id                       unsigned integer
host_name                     string
is_flapping                   boolean
last_check                    time
last_hard_state               short integer
last_hard_state_change        time
last_notification             time
last_state_change             time
last_time_critical            time
last_time_ok                  time
last_time_unknown             time
last_time_warning             time
last_update                   time
latency                       real
max_check_attempts            short integer
modified_attributes           unsigned integer
next_check                    time
next_notification             time
no_more_notifications         boolean
notification_number           short integer
notifications_enabled         boolean
obsess_over                   boolean
passive_checks_enabled        boolean
percent_state_change          real
retry_interval                real
service_description           string
service_id                    unsigned integer
should_be_scheduled           boolean
state_type                    short integer
check_command                 string
output                        string
perf_data                     string
============================= ================ ============================================= =============

Instance configuration
======================

=============== ================ ============================================ =============
Property        Type             Description                                  Version
=============== ================ ============================================ =============
loaded          boolean          True if the instance loaded successfully.
poller_id       unsigned integer ID of the poller which received a
                                 configuration update request (reload).
=============== ================ ============================================ =============

*******
Storage
*******

Metric
======

This event is generated by a Storage endpoint to notify that a RRD
metric graph should be updated.

============== ================ ======================================== =============
Property       Type             Description                              Version
============== ================ ======================================== =============
ctime          time             Time at which the metric value was
                                generated.
interval       unsigned integer Normal service check interval in
                                seconds.
metric_id      unsigned integer Metric ID (from the metrics table).
name           string           Metric name.
rrd_len        integer          RRD retention length in seconds.
value          real             Metric value.
value_type     short integer    Metric type (1 = counter, 2 = derive,
                                3 = absolute, other = gauge).
is_for_rebuild boolean          Set to true when a graph is being
                                rebuild (see the rebuild event).
host_id        unsigned integer The id of the host this metric is        Since 3.0.0
                                attached to.
service_id     unsigned integer The id of the service this metric is     Since 3.0.0
                                attached to.
============== ================ ======================================== =============

Rebuild
=======

Rebuild events are generated when a Storage endpoint detects that some
graph should be rebuild. It first sends a rebuild start event
(end = false), then metric values (metric event with is_for_rebuild set
to true) and finally a rebuild end event (end = true).

============== ================ ============================================== =============
Property       Type             Description                                    Version
============== ================ ============================================== =============
end            boolean          End flag. Set to true if rebuild is starting,
                                false if it is ending.
id             unsigned integer ID of metric to rebuild if is_index is false,
                                or ID of index to rebuild (status graph) if
                                is_index is true.
is_index       boolean          Index flag. Rebuild index (status) if true,
                                rebuild metric if false.
============== ================ ============================================== =============

Remove graph
============

A Storage endpoint generates a remove graph event when some graph must
be deleted.

============== ================ ============================================== =============
Property       Type             Description                                    Version
============== ================ ============================================== =============
id             unsigned integer Index ID (is_index = true) or metric ID
                                (is_index = false) to remove.
is_index       boolean          Index flag. If true, a index (status) graph
                                will be deleted. If false, a metric graph will
                                be deleted.
============== ================ ============================================== =============

Status
======

============== ================ ======================================== =============
Property       Type             Description                              Version
============== ================ ======================================== =============
ctime          time             Time at which the status was generated.
index_id       unsigned integer Index ID.
interval       unsigned integer Normal service check interval in
                                seconds.
rrd_len        time             RRD retention in seconds.
state          short integer    Service state.
is_for_rebuild boolean          Set to true when a graph is being
                                rebuild (see the rebuild event).
============== ================ ======================================== =============

Metric Mapping
==============

============== ================ ======================================== =============
Property       Type             Description                              Version
============== ================ ======================================== =============
index_id       unsigned integer Index ID.
metric_d       unsigned integer Index ID.
============== ================ ======================================== =============

Index Mapping
=============

================= ================ ======================================== =============
Property          Type             Description                              Version
================= ================ ======================================== =============
index_id          unsigned integer Index ID.
host_id           unsigned integer Index ID.
service_id        unsigned integer Index ID.
================= ================ ======================================== =============

***********
Correlation
***********

Engine state
============

Engine state events are sent when the correlation engine starts or stops.

================= ================ ======================================================= =============
Property          Type             Description                                             Version
================= ================ ======================================================= =============
started           boolean          True if the correlation engine is starting, false if it
                                   is stopping.
================= ================ ======================================================= =============

State
==========

================= ================ ========================================= =============
Property          Type             Description                               Version
================= ================ ========================================= =============
ack_time          time
current_state     integer
end_time          time
host_id           unsigned integer
in_downtime       boolean
service_id        unsigned integer 0 for a host.
start_time        time
================= ================ ========================================= =============

Issue
=====

================= ================ ============================================ =============
Property          Type             Description                                  Version
================= ================ ============================================ =============
ack_time          time
end_time          time
host_id           unsigned integer
service_id        unsigned integer
start_time        time
================= ================ ============================================ =============

Issue parent
============

================= ================ ===================================== =============
Property          Type             Description                           Version
================= ================ ===================================== =============
child_host_id     unsigned integer
child_service_id  unsigned integer
child_start_time  time
end_time          time
parent_host_id    unsigned integer
parent_service_id unsigned integer
parent_start_time time
start_time        time
================= ================ ===================================== =============

Log issue
=========

================= ================ ===================================== =============
Property          Type             Description                           Version
================= ================ ===================================== =============
log_ctime         time
host_id           unsigned integer
service_id        unsigned integer
issue_start_time  time
================= ================ ===================================== =============

****
BBDO
****

Version response
================

=============== ================ =============================================== =============
Property        Type             Description                                     Version
=============== ================ =============================================== =============
bbdo_major      short integer    BBDO protocol major used by the peer sending
                                 this *version_response* packet. The sole
                                 current protocol version is 1.0.0.
bbdo_minor      short integer    BBDO protocol minor used by the peer sending
                                 this *version_response* packet.
bbdo_patch      short integer    BBDO protocol patch used by the peer sending
                                 this *version_response* packet.
extensions      string           Space-separated string of extensions supported
                                 by the peer sending this *version_response*
                                 packet.
=============== ================ =============================================== =============

Ack
===

=================== ================ =============================================== =============
Property            Type             Description                                     Version
=================== ================ =============================================== =============
acknowledged events unsigned integer Number of acknowledged events. Only used by
                                     "smart" clients (i.e able to acknowledge
                                     events).
                                     Not to be used by dumb clients.
=================== ================ =============================================== =============

*******
BAM
*******

BA status event
===============

This event is sent when a BA's status changed.

========================= ================ =============================================== =============
Property                  Type             Description                                     Version
========================= ================ =============================================== =============
ba_id                     unsigned integer The id of the BA.                               Since 2.8.0
                                                                                           (BBDO 1.2.0).
in_downtime               boolean          True of the BA is in downtime.                  Since 2.8.0
                                                                                           (BBDO 1.2.0).
last_state_change         time             The time of the last state change of the BA.    Since 2.8.0
                                                                                           (BBDO 1.2.0).
level_acknowledgement     real             The acknowledgment level of the BA.             Since 2.8.0
                                                                                           (BBDO 1.2.0).
level_downtime            real             The downtime level of the BA.                   Since 2.8.0
                                                                                           (BBDO 1.2.0).
level_nominal             real             The nominal level of the BA.                    Since 2.8.0
                                                                                           (BBDO 1.2.0).
state                     short integer    The state of the BA.                            Since 2.8.0
                                                                                           (BBDO 1.2.0).
state_changed             boolean          True if the state of the BA just changed.       Since 2.8.0
                                                                                           (BBDO 1.2.0).
========================= ================ =============================================== =============

KPI status event
================

This event is sent when a KPI's status changed.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
kpi_id                       unsigned integer The id of the KPI.                              Since 2.8.0
                                                                                              (BBDO 1.2.0).
in_downtime                  bool             True if the KPI is in downtime.
level_acknowledgement_hard   real             The hard acknowledgement level of the KPI.      Since 2.8.0
                                                                                              (BBDO 1.2.0).
level_acknowledgement_soft   real             The soft acknowledgement level of the KPI.      Since 2.8.0
                                                                                              (BBDO 1.2.0).
level_downtime_hard          real             The hard downtime level of the KPI.             Since 2.8.0
                                                                                              (BBDO 1.2.0).
level_downtime_soft          real             The soft downtime level of the KPI.             Since 2.8.0
                                                                                              (BBDO 1.2.0).
level_nominal_hard           real             The hard nominal level of the KPI.              Since 2.8.0
                                                                                              (BBDO 1.2.0).
level_nominal_soft           real             The soft nominal level of the KPI.              Since 2.8.0
                                                                                              (BBDO 1.2.0).
state_hard                   short integer    The hard state of the KPI.                      Since 2.8.0
                                                                                              (BBDO 1.2.0).
state_soft                   short integer    The soft state of the KPI.                      Since 2.8.0
                                                                                              (BBDO 1.2.0).
last_state_change            time             The time of the last state change of the KPI.   Since 2.8.0
                                                                                              (BBDO 1.2.0).
last_impact                  real             The last impact of the KPI.                     Since 2.8.0
                                                                                              (BBDO 1.2.0).
valid                        bool             True if the KPi is valid.
============================ ================ =============================================== =============

Meta service status event
=========================

This event is sent when a meta service's status changed.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
meta_service_id              unsigned integer The id of the meta service.                     Since 2.8.0
                                                                                              (BBDO 1.2.0).
value                        real             The value of the meta service.                  Since 2.8.0
                                                                                              (BBDO 1.2.0).
state_changed                boolean          True if the state just changed.                 Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

BA-event event
==============

This event is sent when a new BA event is opened, or an old one is closed.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
ba_id                        unsigned integer The id of the BA.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
first_level                  real             The first level of the BA event.                Since 2.8.0
                                                                                              (BBDO 1.2.0).
end_time                     time             The end_time of the event. 0 or (time)-1 for    Since 2.8.0
                                              an opened event.                                (BBDO 1.2.0).
in_downtime                  boolean          True if BA was in downtime during the BA event. Since 2.8.0
                                                                                              (BBDO 1.2.0).
start_time                   time             The start_time of the event.                    Since 2.8.0
                                                                                              (BBDO 1.2.0).
status                       short integer    The status of the BA during the event.          Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

KPI-event event
===============

This event is sent when a new KPI event is opened, or an old one is closed.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
kpi_id                       unsigned integer The id of the KPI.                              Since 2.8.0
                                                                                              (BBDO 1.2.0).
end_time                     time             The end_time of the event. 0 or (time)-1 for    Since 2.8.0
                                              an opened event.                                (BBDO 1.2.0).
impact_level                 integer          The level of the impact.                        Since 2.8.0
                                                                                              (BBDO 1.2.0).
in_downtime                  boolean          True if BA was in downtime during the BA event. Since 2.8.0
                                                                                              (BBDO 1.2.0).
first_output                 string           The first output of the KPI during the event.   Since 2.8.0
                                                                                              (BBDO 1.2.0).
perfdata                     string           The first perfdata of the KPI during the event. Since 2.8.0
                                                                                              (BBDO 1.2.0).
start_time                   time             The start_time of the event.                    Since 2.8.0
                                                                                              (BBDO 1.2.0).
status                       short integer    The status of the BA during the event.          Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

BA duration event event
=======================

This event is sent when a new BA duration event is computed by BAM broker.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
ba_id                        unsigned integer The id of the BA.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
real_start_time              time             The first level of the BA event.                Since 2.8.0
                                                                                              (BBDO 1.2.0).
end_time                     time             The end_time of the event, in the given         Since 2.8.0
                                              timeperiod.                                     (BBDO 1.2.0).
start_time                   time             The start_time of the event, in the given       Since 2.8.0
                                              timeperiod.                                     (BBDO 1.2.0).
duration                     unsigned integer end_time - start_time.                          Since 2.8.0
                                                                                              (BBDO 1.2.0).
sla_duration                 unsigned integer The duration of the event in the given          Since 2.8.0
                                              timperiod.                                      (BBDO 1.2.0).
timeperiod_is_default        boolean          True if the timeperiod if the default for       Since 2.8.0
                                              this BA.                                        (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension BA
============

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
ba_id                        unsigned integer The id of the BA.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
ba_name                      string           The name of the BA.                             Since 2.8.0
                                                                                              (BBDO 1.2.0).
ba_description               string           The description of the BA.                      Since 2.8.0
                                                                                              (BBDO 1.2.0).
sla_month_percent_crit       real                                                             Since 2.8.0
                                                                                              (BBDO 1.2.0).
sla_month_percent_warn       real                                                             Since 2.8.0
                                                                                              (BBDO 1.2.0).
sla_month_duration_crit      unsigned integer                                                 Since 2.8.0
                                                                                              (BBDO 1.2.0).
sla_month_duration_warn      unsigned integer                                                 Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension KPI
=============

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
kpi_id                       unsigned integer The id of the KPI.                              Since 2.8.0
                                                                                              (BBDO 1.2.0).
ba_id                        unsigned integer The id of the parent BA of this KPI.            Since 2.8.0
                                                                                              (BBDO 1.2.0).
ba_name                      string           The name of the parent BA of this KPI.          Since 2.8.0
                                                                                              (BBDO 1.2.0).
host_id                      unsigned integer The id of the host associated with this KPI     Since 2.8.0
                                              for service KPI.                                (BBDO 1.2.0).
host_name                    string           The name of the host associated with this KPI   Since 2.8.0
                                              for service KPI.                                (BBDO 1.2.0)
service_id                   unsigned integer The id of the service associated with this KPI  Since 2.8.0
                                              for service KPI.                                (BBDO 1.2.0).
service_description          string           The description of the service associated with  Since 2.8.0
                                              this KPI for service KPI.                       (BBDO 1.2.0).
kpi_ba_id                    unsigned integer The id of the BA associated with this KPI for   Since 2.8.0
                                              BA KPI.                                         (BBDO 1.2.0).
kpi_ba_name                  string           The name of the BA associated with this KPI     Since 2.8.0
                                              for BA KPI.                                     (BBDO 1.2.0).
meta_service_id              unsigned int     The id of the meta-service associated with this Since 2.8.0
                                              KPI for meta-service KPI.                       (BBDO 1.2.0).
meta_service_name            string           The name of the meta-service associated with    Since 2.8.0
                                              this KPI for meta-service KPI.                  (BBDO 1.2.0).
boolean_id                   unsigned int     The id of the boolean expression associated     Since 2.8.0
                                              with this KPI for boolean KPI.                  (BBDO 1.2.0).
boolean_name                 string           The name of the boolean expression              Since 2.8.0
                                              associated with this KPI for boolean KPI.       (BBDO 1.2.0).
impact_warning               real             The impact of a warning state for this KPI.     Since 2.8.0
                                                                                              (BBDO 1.2.0).
impact_critical              real             The impact of a critical state for this KPI.    Since 2.8.0
                                                                                              (BBDO 1.2.0).
impact_unknown               real             The impact of a unknown state for this KPI.     Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension BA BV relation
========================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
ba_id                        unsigned integer The id of the BA.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
bv_id                        unsigned integer The id of the BV.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension BV
============

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
bv_id                        unsigned integer The id of the BV.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
bv_name                      string           The name of the BV.                             Since 2.8.0
                                                                                              (BBDO 1.2.0).
bv_description               string           The description of the BV.                      Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension table signal
======================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

This signal is sent before the dump of all the dimensions, and again at the end of the dump.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
update_started               boolean          True if this is the start of the dump,          Since 2.8.0
                                              false if it's the end.                          (BBD0 1.2.0).
============================ ================ =============================================== =============

Rebuild signal
==============

This event is sent when a rebuild of the event durations and availabilities
is asked to the BAM broker endpoint.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
bas_to_rebuild               string           A string containing the id of all the BAs       Since 2.8.0
                                              to rebuild, separated by a comma and a space    (BBDO 1.2.0).
                                              (i.e "1, 5, 8, 12").
============================ ================ =============================================== =============

Dimension timeperiod
====================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
tp_id                        unsigned integer The id of the timeperiod.                       Since 2.8.0
                                                                                              (BBDO 1.2.0).
name                         string           The name of the timeperiod.                     Since 2.8.0
                                                                                              (BBDO 1.2.0).
monday                       string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
tuesday                      string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
wednesday                    string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
thursday                     string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
friday                       string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
saturday                     string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
sunday                       string           The timeperiod rule for this day.               Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension BA timeperiod relation
================================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
ba_id                        unsigned integer The id of the BA.                               Since 2.8.0
                                                                                              (BBDO 1.2.0).
timeperiod_id                unsigned integer The id of the timeperiod.                       Since 2.8.0
                                                                                              (BBDO 1.2.0).
is_default                   boolean          True if the timeperiod is the default one for   Since 2.8.0
                                              this BA.                                        (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension timeperiod exception
==============================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
timeperiod_id                unsigned integer The id of the timeperiod having this exception. Since 2.8.0
daterange                    string           A string containing the date of the range.      Since 2.8.0
timerange                    string           A string containing the time of the range.      Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Dimension timeperiod exclusion
==============================

This event is part of the dimension (i.e configuration) dump occuring at
startup and after each BAM configuration reload.

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
timeperiod_id                unsigned integer The id of the timeperiod having this exclusion. Since 2.8.0
                                                                                              (BBDO 1.2.0).
excluded_timeperiod_id       unsigned integer The id of the excluded timeperiod.              Since 2.8.0
                                                                                              (BBDO 1.2.0).
============================ ================ =============================================== =============

Inherited downtime
==================

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
bad_id                       unsigned integer The id of the BA in downtime.
in_downtime                  boolean          True if the BA is in downtime.
============================ ================ =============================================== =============

******
Extcmd
******

Command request
===============

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
command                      string           The command request.
endp                         string           The endpoint this command is destined to.
uuid                         string           The uuid of this request.
with_partial_result          boolean          True if the command should be answered
                                              with partial result.
============================ ================ =============================================== =============

Command result
==============

============================ ================ =============================================== =============
Property                     Type             Description                                     Version
============================ ================ =============================================== =============
code                         integer          The return code of this command.
uuid                         string           The uuid of the request this command is the
                                              result of.
msg                          string           The string message of the command result.
============================ ================ =============================================== =============
