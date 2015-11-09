--
--  Base performance data index.
--
CREATE TABLE rt_index_data (
  index_id int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  check_interval int default NULL,
  hidden boolean NOT NULL default 0,
  host_name varchar(255) default NULL,
  locked boolean NOT NULL default 0,
  must_be_rebuild smallint NOT NULL default 0,
  rrd_retention int default NULL,
  service_description varchar(255) default NULL,
  special boolean NOT NULL default 0,
  storage_type smallint NOT NULL default 2,
  to_delete boolean NOT NULL default 0,
  trashed boolean NOT NULL default 0,

  PRIMARY KEY (index_id),
  UNIQUE (host_id, service_id),
  INDEX (host_id),
  INDEX (host_name),
  INDEX (must_be_rebuild),
  INDEX (service_description),
  INDEX (service_id),
  INDEX (trashed)
);
CREATE SEQUENCE index_data_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER index_data_trigger
BEFORE INSERT ON index_data
FOR EACH ROW
BEGIN
  SELECT index_data_seq.nextval INTO :NEW.index_id FROM dual;
END;
/
