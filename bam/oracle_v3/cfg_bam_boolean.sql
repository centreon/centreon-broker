--
-- BAM boolean expressions.
--
CREATE TABLE cfg_bam_boolean (
  boolean_id int NOT NULL,
  name varchar(255) NOT NULL,

  expression text NOT NULL,
  bool_state boolean NOT NULL default 1,
  activate boolean NOT NULL default 0,

  PRIMARY KEY (boolean_id),
  UNIQUE (name)
);
CREATE SEQUENCE cfg_bam_boolean_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_boolean_trigger
BEFORE INSERT ON cfg_bam_boolean
FOR EACH ROW
BEGIN
  SELECT cfg_bam_boolean_seq.nextval INTO :NEW.boolean_id FROM dual;
END;
/
