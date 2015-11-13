--
-- Custom variables.
--
CREATE TABLE customvariables (
  customvariable_id int NOT NULL,
  host_id int default NULL,
  name varchar(255) default NULL,
  service_id int default NULL,

  default_value varchar(255) default NULL,
  modified char(1) default NULL,
  type smallint default NULL,
  update_time int default NULL,
  value varchar(255) default NULL,

  PRIMARY KEY (customvariable_id),
  UNIQUE (host_id, name, service_id)
);
CREATE SEQUENCE customvariables_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER customvariables_trigger
BEFORE INSERT ON customvariables
FOR EACH ROW
BEGIN
  SELECT customvariables_seq.nextval INTO :NEW.customvariable_id FROM dual;
END;
/
