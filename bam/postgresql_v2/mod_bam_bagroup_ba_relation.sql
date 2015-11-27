--
-- BA / Group relations.
--
CREATE TABLE mod_bam_bagroup_ba_relation (
  id_bgr serial,
  id_ba int NOT NULL,
  id_ba_group int NOT NULL,

  PRIMARY KEY (id_bgr),
  FOREIGN KEY (id_ba) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba_group) REFERENCES mod_bam_ba_groups (id_ba_group)
    ON DELETE CASCADE
);
