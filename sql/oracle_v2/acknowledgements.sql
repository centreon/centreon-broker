--
-- Holds acknowledgedments information.
--
CREATE TABLE acknowledgements (
  acknowledgement_id int NOT NULL,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  author varchar(64) default NULL,
  comment_data varchar(255) default NULL,
  deletion_time int default NULL,
  instance_id int default NULL,
  notify_contacts char(1) default NULL,
  persistent_comment char(1) default NULL,
  state smallint default NULL,
  sticky char(1) default NULL,
  type smallint default NULL,

  PRIMARY KEY (acknowledgement_id),
  INDEX (host_id),
  INDEX (instance_id),
  INDEX (entry_time),
  UNIQUE (entry_time, host_id, service_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE SET NULL
);
CREATE SEQUENCE acknowledgements_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER acknowledgements_trigger
BEFORE INSERT ON acknowledgements
FOR EACH ROW
BEGIN
  SELECT acknowledgements_seq.nextval INTO :NEW.acknowledgement_id FROM dual;
END;
/
