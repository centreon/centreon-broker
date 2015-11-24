--
-- Relations between BA and BV.
--
CREATE TABLE mod_bam_reporting_relations_ba_bv (
  ba_bv_id int NOT NULL,
  bv_id int NOT NULL,
  ba_id int NOT NULL,

  PRIMARY KEY (ba_bv_id),
  FOREIGN KEY (bv_id) REFERENCES mod_bam_reporting_bv (bv_id)
    ON DELETE CASCADE,
  FOREIGN KEY (ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE mod_bam_reporting_relations_ba_bv_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_reporting_relations_ba_bv_trigger
BEFORE INSERT ON mod_bam_reporting_relations_ba_bv
FOR EACH ROW
BEGIN
  SELECT mod_bam_reporting_relations_ba_bv_seq.nextval INTO :NEW.ba_bv_id FROM dual;
END;
/
