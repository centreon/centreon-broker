--
-- Impacts of KPI / boolean expressions.
--
CREATE TABLE cfg_bam_impacts (
  id_impact int NOT NULL,
  impact float NOT NULL,

  PRIMARY KEY (id_impact)
);
CREATE SEQUENCE cfg_bam_impacts_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_impacts_trigger
BEFORE INSERT ON cfg_bam_impacts
FOR EACH ROW
BEGIN
  SELECT cfg_bam_impacts_seq.nextval INTO :NEW.id_impact FROM dual;
END;
/
