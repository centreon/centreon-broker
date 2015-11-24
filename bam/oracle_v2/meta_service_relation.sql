--
-- Meta Services Relationships.
--
CREATE TABLE meta_service_relation (
  msr_id int NOT NULL,

  meta_id int default NULL,
  host_id int default NULL,
  metric_id int default NULL,
  msr_comment text,
  activate enum('0','1') default NULL,

  PRIMARY KEY (msr_id),
  FOREIGN KEY (meta_id) REFERENCES meta_service (meta_id) ON DELETE CASCADE
);
CREATE SEQUENCE meta_service_relation_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER meta_service_relation_trigger
BEFORE INSERT ON meta_service_relation
FOR EACH ROW
BEGIN
  SELECT meta_service_relation_seq.nextval INTO :NEW.msr_id FROM dual;
END;
/
