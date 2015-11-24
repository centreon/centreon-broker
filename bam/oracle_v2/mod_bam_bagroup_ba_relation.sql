--
-- BA / Group relations.
--
CREATE TABLE mod_bam_bagroup_ba_relation (
  id_bgr int NOT NULL,
  id_ba int NOT NULL,
  id_ba_group int NOT NULL,

  PRIMARY KEY (id_bgr),
  FOREIGN KEY (id_ba) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba_group) REFERENCES mod_bam_ba_groups (id_ba_group)
    ON DELETE CASCADE
);
CREATE SEQUENCE mod_bam_bagroup_ba_relation_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_bagroup_ba_relation_trigger
BEFORE INSERT ON mod_bam_bagroup_ba_relation
FOR EACH ROW
BEGIN
  SELECT mod_bam_bagroup_ba_relation_seq.nextval INTO :NEW.id_bgr FROM dual;
END;
/
