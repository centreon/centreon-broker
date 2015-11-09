--
-- Correlated issues.
--
CREATE TABLE rt_issues (
  issue_id int NOT NULL,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  INDEX (start_time),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE issues_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER issues_trigger
BEFORE INSERT ON rt_issues
FOR EACH ROW
BEGIN
  SELECT issues_seq.nextval INTO :NEW.issue_id FROM dual;
END;
/
