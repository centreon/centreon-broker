--
-- Centreon Engine modules.
--
CREATE TABLE modules (
  module_id int NOT NULL,
  instance_id int NOT NULL,

  args varchar(255) default NULL,
  filename varchar(255) default NULL,
  loaded char(1) default NULL,
  should_be_loaded char(1) default NULL,

  PRIMARY KEY (module_id),
  INDEX (instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE modules_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER modules_trigger
BEFORE INSERT ON modules
FOR EACH ROW
BEGIN
  SELECT modules_seq.nextval INTO :NEW.module_id FROM dual;
END;
/
