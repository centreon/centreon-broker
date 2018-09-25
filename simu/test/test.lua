--package.path = package.path .. ";/home/admin/?.lua"

local export_conf = require 'export_conf'

local simu = {
  input_file = "/tmp/studio.txt",
}

function init(conf)
  broker_log:set_parameters(3, "/tmp/simu.log")

  -- We are waiting for a string here
  if conf.input_file then
    simu.input_file = conf.input_file
  end
end

function read()
  local retval = {"obsess_over_host":true,"no_more_notifications":false,"check_attempt":1,"icon_image_alt":"","enabled":true,"host_id":19,"state":0,"last_update":1536674331,"address":"10.0.2.15","last_check":1536674255,"notify_on_recovery":true,"freshness_threshold":0,"default_active_checks":true,"notification_period":"","action_url":"","check_type":0,"retain_nonstatus_information":true,"last_time_down":1536046683,"last_time_up":1536674255,"instance_id":1,"check_period":"24x7","first_notification_delay":0,"last_hard_state":0,"event_handler":"","element":12,"stalk_on_down":false,"latency":0,"low_flap_threshold":0,"last_state_change":1536268488,"timezone":":Europe/Paris","next_check":1536674378,"should_be_scheduled":true,"flap_detection":true,"notify_on_down":true,"notification_interval":30,"flap_detection_on_down":true,"icon_image":"ppm/applications-monitoring-centreon-central-App-Centreon-64.png","perfdata":"rta=0,022ms","notify_on_downtime":true,"output":"OK - 10.0.2.15: rta 0,022ms, lost 0%\n","check_command":"base_host_alive","name":"central_4","notify_on_flapping":true,"statusmap_image":"","stalk_on_up":false,"execution_time":0,"acknowledged":false,"stalk_on_unreachable":false,"active_checks":true,"default_notify":true,"retry_interval":1,"max_check_attempts":3,"notes":"","default_flap_detection":true,"default_passive_checks":false,"last_hard_state_change":1536268488,"passive_checks":false,"display_name":"central_4","category":1,"high_flap_threshold":0,"check_interval":5,"alias":"central","notify_on_unreachable":true,"checked":true,"type":65548,"acknowledgement_type":0,"state_type":1,"notes_url":"","notify":true,"notification_number":0,"percent_state_change":0,"flap_detection_on_unreachable":true,"flapping":false,"flap_detection_on_up":true,"check_freshness":false,"scheduled_downtime_depth":0,"default_event_handler_enabled":true,"retain_status_information":true,"event_handler_enabled":true}
  return retval
end
