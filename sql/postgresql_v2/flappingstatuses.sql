--
-- Historization of flapping statuses.
--
CREATE TABLE flappingstatuses (
  flappingstatus_id serial,
  host_id int default NULL,
  service_id int default NULL,
  event_time int default NULL,

  comment_time int default NULL,
  event_type smallint default NULL,
  high_threshold double precision default NULL,
  internal_comment_id int default NULL,
  low_threshold double precision default NULL,
  percent_state_change double precision default NULL,
  reason_type smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (flappingstatus_id),
  UNIQUE (host_id, service_id, event_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;
