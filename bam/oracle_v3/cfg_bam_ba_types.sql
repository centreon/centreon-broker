--
-- Business Activities types.
--
CREATE TABLE cfg_bam_ba_types (
  ba_type_id int NOT NULL,
  name varchar(255) default NULL,
  slug varchar(255) default NULL,
  description varchar(255) default NULL,

  PRIMARY KEY (ba_type_id),
  KEY (name)
);
CREATE SEQUENCE cfg_bam_ba_types_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_ba_types_trigger
BEFORE INSERT ON cfg_bam_ba_types
FOR EACH ROW
BEGIN
  SELECT cfg_bam_ba_types_seq.nextval INTO :NEW.ba_type_id FROM dual;
END;
/
