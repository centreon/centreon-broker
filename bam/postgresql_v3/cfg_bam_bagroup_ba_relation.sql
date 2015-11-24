--
-- BA / Group relations.
--
CREATE TABLE cfg_bam_bagroup_ba_relation (
  id_ba int NOT NULL,
  id_ba_group int NOT NULL,

  UNIQUE (id_ba, id_ba_group),
  FOREIGN KEY (id_ba) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba_group) REFERENCES cfg_bam_ba_groups (id_ba_group)
    ON DELETE CASCADE
);
