--
-- Event handlers.
--
CREATE TABLE eventhandlers (
  eventhandler_id int NOT NULL,
  host_id int default NULL,
  service_id int default NULL,
  start_time int default NULL,

  command_args varchar(255) default NULL,
  command_line varchar(255) default NULL,
  early_timeout char(1) default NULL,
  end_time int default NULL,
  eventhandler_type smallint default NULL,
  execution_time double precision default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,

  PRIMARY KEY (eventhandler_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE eventhandlers_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER eventhandlers_trigger
BEFORE INSERT ON eventhandlers
FOR EACH ROW
BEGIN
  SELECT eventhandlers_seq.nextval INTO :NEW.eventhandler_id FROM dual;
END;
/
