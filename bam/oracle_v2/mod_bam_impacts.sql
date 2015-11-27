--
-- Impacts of KPI / boolean expressions.
--
CREATE TABLE mod_bam_impacts (
  id_impact int NOT NULL,
  impact float NOT NULL,

  PRIMARY KEY (id_impact)
);
CREATE SEQUENCE mod_bam_impacts_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_impacts_trigger
BEFORE INSERT ON mod_bam_impacts
FOR EACH ROW
BEGIN
  SELECT mod_bam_impacts_seq.nextval INTO :NEW.id_impact FROM dual;
END;
/
