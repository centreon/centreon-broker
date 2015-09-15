-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Notification             --
-- ------------------------------------

-- cfg_notification_methods
-- cfg_notification_rules
-- rt_notification_rules

--
-- Notification methods.
--
CREATE TABLE cfg_notification_methods (
  method_id int NOT NULL auto_increment,
  name varchar (255) NOT NULL,

  command_id int NOT NULL,
  `interval` int default NULL,
  status varchar (32) default NULL,
  types varchar (32) default NULL,
  start int default NULL,
  end int default NULL,

  PRIMARY KEY (method_id),
  UNIQUE (name),
  FOREIGN KEY (command_id) REFERENCES cfg_commands (command_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Notification rules (configuration).
--
CREATE TABLE cfg_notification_rules (
  rule_id int NOT NULL auto_increment,

  owner_id int NOT NULL,
  method_id int NOT NULL,
  host_id int default NULL,
  service_id int default NULL,
  hg_id int default NULL,
  sg_id int default NULL,
  contact_id int default NULL,
  timeperiod_id int default NULL,
  enabled boolean NOT NULL default 1,

  PRIMARY KEY (rule_id),
  FOREIGN KEY (owner_id) REFERENCES cfg_contacts (contact_id)
    ON DELETE RESTRICT,
  FOREIGN KEY (method_id) REFERENCES cfg_notification_methods (method_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES cfg_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (service_id) REFERENCES cfg_services (service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (hg_id) REFERENCES cfg_hostgroups (hg_id)
    ON DELETE CASCADE,
  FOREIGN KEY (sg_id) REFERENCES cfg_servicegroups (sg_id)
    ON DELETE CASCADE,
  FOREIGN KEY (contact_id) REFERENCES cfg_contacts (contact_id)
    ON DELETE CASCADE,
  FOREIGN KEY (timeperiod_id) REFERENCES cfg_timeperiods (tp_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Notification rules (expanded).
--
CREATE TABLE rt_notification_rules (
  rule_id int NOT NULL,
  method_id int NOT NULL,
  timeperiod_id int default NULL,
  contact_id int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  FOREIGN KEY (rule_id) REFERENCES cfg_notification_rules (rule_id)
    ON DELETE CASCADE,
  FOREIGN KEY (method_id) REFERENCES cfg_notification_methods (method_id)
    ON DELETE CASCADE,
  FOREIGN KEY (timeperiod_id) REFERENCES cfg_timeperiods (tp_id)
    ON DELETE CASCADE,
  FOREIGN KEY (contact_id) REFERENCES cfg_contacts (contact_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES cfg_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (service_id) REFERENCES cfg_services (service_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
