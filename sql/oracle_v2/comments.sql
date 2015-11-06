--
-- Holds comments information.
--
CREATE TABLE comments (
  comment_id int NOT NULL,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  author varchar(64) default NULL,
  data clob default NULL,
  deletion_time int default NULL,
  entry_type smallint default NULL,
  expire_time int default NULL,
  expires char(1) default NULL,
  instance_id int default NULL,
  internal_id int NOT NULL,
  persistent char(1) default NULL,
  source smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (comment_id),
  INDEX (internal_id),
  INDEX (host_id),
  INDEX (instance_id),
  UNIQUE (entry_time, host_id, service_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE SET NULL
);
CREATE SEQUENCE comments_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER comments_trigger
BEFORE INSERT ON comments
FOR EACH ROW
BEGIN
  SELECT comments_seq.nextval INTO :NEW.comment_id FROM dual;
END;
/
