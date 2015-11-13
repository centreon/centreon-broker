--
--  Host states.
--
CREATE TABLE hoststateevents (
  hoststateevent_id serial,
  host_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime boolean NOT NULL,
  last_update tinyint NOT NULL default 0,
  state tinyint NOT NULL,

  PRIMARY KEY (hoststateevent_id),
  UNIQUE (host_id, start_time),
  INDEX (start_time),
  INDEX (end_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);
