--
--  Host states.
--
CREATE TABLE hoststateevents (
  hoststateevent_id int NOT NULL,
  host_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime char(1) NOT NULL,
  last_update tinyint NOT NULL default 0,
  state tinyint NOT NULL,

  PRIMARY KEY (hoststateevent_id),
  UNIQUE (host_id, start_time),
  INDEX (start_time),
  INDEX (end_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE hoststateevents_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER hoststateevents_trigger
BEFORE INSERT ON hoststateevents
FOR EACH ROW
BEGIN
  SELECT hoststateevents_seq.nextval INTO :NEW.hoststateevent_id FROM dual;
END;
/
