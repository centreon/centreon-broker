--
-- BA Groups (aka BV).
--
CREATE TABLE cfg_bam_ba_groups (
  id_ba_group int NOT NULL,
  ba_group_name varchar(255) NOT NULL,

  ba_group_description varchar(255) default NULL,
  visible enum('0', '1') NOT NULL default '1',

  PRIMARY KEY (id_ba_group),
  UNIQUE (ba_group_name)
);
CREATE SEQUENCE cfg_bam_ba_groups_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_ba_groups_trigger
BEFORE INSERT ON cfg_bam_ba_groups
FOR EACH ROW
BEGIN
  SELECT cfg_bam_ba_groups_seq.nextval INTO :NEW.id_ba_group FROM dual;
END;
/
