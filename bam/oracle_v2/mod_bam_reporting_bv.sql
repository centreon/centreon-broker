--
-- Business Views.
--
CREATE TABLE mod_bam_reporting_bv (
  bv_id int NOT NULL,
  bv_name varchar(45) default NULL,

  bv_description text default NULL,

  PRIMARY KEY (bv_id),
  UNIQUE (bv_name)
);
CREATE SEQUENCE mod_bam_reporting_bv_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_reporting_bv_trigger
BEFORE INSERT ON mod_bam_reporting_bv
FOR EACH ROW
BEGIN
  SELECT mod_bam_reporting_bv_seq.nextval INTO :NEW.bv_id FROM dual;
END;
/
