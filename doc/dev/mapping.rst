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

Property Type Description
acknowledgement_type
author
comment
deletion_time
entry_time
host_id
instance_id
is_sticky
notify_contacts
persistent_comment
service_id
state

Comment
=======

Property Type Description
author
comment_type
deletion_time
entry_time
entry_type
expire_time
expires
host_id
instance_id
internal_id
persistent
service_id
source
data

Custom variable
===============

Property Type Description
host_id
modified
name
service_id
update_time
var_type
value

Custom variable status
======================

Property Type Description
host_id
modified
name
service_id
update_time
value

Downtime
========

Property Type Description
actual_end_time
actual_start_time
author
downtime_type
deletion_time
duration
end_time
entry_time
fixed
host_id
instance_id
internal_id
service_id
start_time
triggered_by
was_cancelled
was_started
comment

Event handler
=============

Property Type Description
early_timeout
end_time
execution_time
handler_type
host_id
return_code
service_id
start_time
state
state_type
timeout
command_args
command_line
output

Flapping status
===============

Property Type Description
comment_time
event_time
event_type
flapping_type
high_threshold
host_id
internal_comment_id
low_threshold
percent_state_change
reason_type
service_id

Host
====

Property Type Description
acknowledgement_type
action_url
active_checks_enabled
address
alias
check_freshness
check_interval
check_period
check_type
current_check_attempt
current_notification_number
current_state
default_active_checks_enabled
default_event_handler_enabled
default_failure_prediction
default_flap_detection_enabled
default_notifications_enabled
default_passive_checks_enabled
default_process_perf_data
display_name
enabled
event_handler
event_handler_enabled
execution_time
failure_prediction_enabled
first_notification_delay
flap_detection_enabled
flap_detection_on_down
flap_detection_on_unreachable
flap_detection_on_up
freshness_threshold
has_been_checked
high_flap_threshold
host_name
icon_image
icon_image_alt
host_id
instance_id
is_flapping
last_check
last_hard_state
last_hard_state_change
last_notification
last_state_change
last_time_down
last_time_unreachable
last_time_up
last_update
latency
low_flap_threshold
max_check_attempts
modified_attributes
next_check
next_notification
no_more_notifications
notes
notes_url
notification_interval
notification_period
notifications_enabled
notify_on_down
notify_on_downtime
notify_on_flapping
notify_on_recovery
notify_on_unreachable
obsess_over
passive_checks_enabled
percent_state_change
problem_has_been_acknowledged
process_performance_data
retain_nonstatus_information
retain_status_information
retry_interval
scheduled_downtime_depth
should_be_scheduled
stalk_on_down
stalk_on_unreachable
stalk_on_up
state_type
statusmap_image
check_command
output
perf_data

Host check
==========

Property Type Description
active_checks_enabled
check_type
host_id
next_check
command_line

Host dependency
===============

Property Type Description
dependency_period
dependent_host_id
enabled
execution_failure_options
inherits_parent
notification_failure_options
host_id

Host group
==========

Property Type Description
action_url
alias
enabled
instance_id
name
notes
notes_url

Host group member
=================

Property Type Description
enabled
group
instance_id
host_id

Host parent
===========

Property Type Description
enabled
host_id
parent_id

Host status
===========

Property Type Description
acknowledgement_type
active_checks_enabled
check_interval
check_period
check_type
current_check_attempt
current_notification_number
current_state
enabled
event_handler
event_handler_enabled
execution_time
failure_prediction_enabled
flap_detection_enabled
has_been_checked
host_id
is_flapping
last_check
last_hard_state
last_hard_state_change
last_notification
last_state_change
last_time_down
last_time_unreachable
last_time_up
last_update
latency
max_check_attempts
modified_attributes
next_check
next_notification
no_more_notifications
notifications_enabled
obsess_over
passive_checks_enabled
percent_state_change
problem_has_been_acknowledged
process_performance_data
retry_interval
scheduled_downtime_depth
should_be_scheduled
state_type
check_command
output
perf_data

Instance
========

Property Type Description
engine
id
name
is_running
pid
program_end
program_start
version

Instance status
===============

Property Type Description
active_host_checks_enabled
active_service_checks_enabled
address
check_hosts_freshness
check_services_freshness
daemon_mode
description
event_handler_enabled
failure_prediction_enabled
flap_detection_enabled
id
last_alive
last_command_check
last_log_rotation
modified_host_attributes
modified_service_attributes
notifications
obsess_over_hosts
obsess_over_services
passive_host_checks_enabled
passive_service_checks_enabled
process_performance_data
global_host_event_handler
global_service_event_handler

Log entry
=========

Property Type Description
c_time
host_id
host_name
instance_name
issue_start_time
log_type
msg_type
notification_cmd
notification_contact
retry
service_description
service_id
status
output

Module
======

Property Type Description
args
enabled
filename
instance_id
loaded
should_be_loaded

Notification
============

Property Type Description
contacts_notified
end_time
escalated
host_id
notification_type
reason_type
service_id
start_time
state
ack_author
ack_data
command_name
contact_name
output

Service
=======

Property Type Description
acknowledgement_type
action_url
active_checks_enabled
check_freshness
check_interval
check_period
check_type
current_check_attempt
current_notification_number
current_state
default_active_checks_enabled
default_event_handler_enabled
default_failure_prediction
default_flap_detection_enabled
default_notifications_enabled
default_passive_checks_enabled
default_process_perf_data
display_name
enabled
event_handler
event_handler_enabled
execution_time
failure_prediction_enabled
failure_prediction_options
first_notification_delay
flap_detection_enabled
flap_detection_on_critical
flap_detection_on_ok
flap_detection_on_unknown
flap_detection_on_warning
freshness_threshold
has_been_checked
high_flap_threshold
host_id
host_name
icon_image
icon_image_alt
service_id
is_flapping
is_volatile
last_check
last_hard_state
last_hard_state_change
last_notification
last_state_change
last_time_critical
last_time_ok
last_time_unknown
last_time_warning
last_update
latency
low_flap_threshold
max_check_attempts
modified_attributes
next_check
next_notification
no_more_notifications
notes
notes_url
notification_interval
notification_period
notifications_enabled
notify_on_critical
notify_on_downtime
notify_on_flapping
notify_on_recovery
notify_on_unknown
notify_on_warning
obsess_over
passive_checks_enabled
percent_state_change
problem_has_been_acknowledged
process_performance_data
retain_nonstatus_information
retain_status_information
retry_interval
scheduled_downtime_depth
service_description
should_be_scheduled
stalk_on_critical
stalk_on_ok
stalk_on_unknown
stalk_on_warning
state_type
check_command
output
perf_data

Service check
=============

Property Type Description
active_checks_enabled
check_type
host_id
next_check
service_id
command_line

Service dependency
==================

Property Type Description
dependency_period
dependent_host_id
dependent_service_id
enabled
execution_failure_options
host_id
inherits_parent
notification_failure_options
service_id

Service group
=============

Property Type Description
action_url
alias
enabled
instance_id
name
notes
notes_url

Service group member
====================

Property Type Description
enabled
group
host_id
instance_id
service_id

Service status
==============

Property Type Description
acknowledgement_type
active_checks_enabled
check_interval
check_period
check_type
current_check_attempt
current_notification_number
current_state
enabled
event_handler
event_handler_enabled
execution_time
failure_prediction_enabled
flap_detection_enabled
has_been_checked
host_id
host_name
is_flapping
last_check
last_hard_state
last_hard_state_change
last_notification
last_state_change
last_time_critical
last_time_ok
last_time_unknown
last_time_warning
last_update
latency
max_check_attempts
modified_attributes
next_check
next_notification
no_more_notifications
notifications_enabled
obsess_over
passive_checks_enabled
percent_state_change
problem_has_been_acknowledged
process_performance_data
retry_interval
scheduled_downtime_depth
service_description
service_id
should_be_scheduled
state_type
check_command
output
perf_data

*******
Storage
*******

Metric
======

This event is generated by a Storage endpoint to notify that a RRD
metric graph should be updated.

============== ================ ========================================
Property       Type             Description
============== ================ ========================================
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
============== ================ ========================================

Rebuild
=======

Rebuild events are generated when a Storage endpoint detects that some
graph should be rebuild. It first sends a rebuild start event
(end = false), then metric values (metric event with is_for_rebuild set
to true) and finally a rebuild end event (end = true).

======== ================ ==============================================
Property Type             Description
======== ================ ==============================================
end      boolean          End flag. Set to true if rebuild is starting,
                          false if it is ending.
id       unsigned integer ID of metric to rebuild if is_index is false,
                          or ID of index to rebuild (status graph) if
                          is_index is true.
is_index boolean          Index flag. Rebuild index (status) if true,
                          rebuild metric if false.
======== ================ ==============================================

Remove graph
============

A Storage endpoint generates a remove graph event when some graph must
be deleted.

======== ================ ==============================================
Property Type             Description
======== ================ ==============================================
id       unsigned integer Index ID (is_index = true) or metric ID
                          (is_index = false) to remove.
is_index boolean          Index flag. If true, a index (status) graph
                          will be deleted. If false, a metric graph will
                          be deleted.
======== ================ ==============================================

Status
======
============== ================ ========================================
Property       Type             Description
============== ================ ========================================
ctime          time             Time at which the status was generated.
index_id       unsigned integer Index ID.
interval       unsigned integer Normal service check interval in
                                seconds.
rrd_len        time             RRD retention in seconds.
state          short integer    Service state.
is_for_rebuild boolean          Set to true when a graph is being
                                rebuild (see the rebuild event).
============== ================ ========================================

***********
Correlation
***********

Engine state
============

Property Type Description
started

Host state
==========

Property Type Description
ack_time
current_state
end_time
host_id
in_downtime
start_time

Issue
=====

Property Type Description
ack_time
end_time
host_id
service_id
start_time

Issue parent
============

Property Type Description
child_host_id
child_service_id
child_start_time
end_time
parent_host_id
parent_service_id
parent_start_time
start_time

Service state
=============

Property Type Description
ack_time
state
end_time
host_id
in_downtime
service_id
start_time
