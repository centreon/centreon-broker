--
-- BA Groups (aka BV).
--
CREATE TABLE mod_bam_ba_groups (
  id_ba_group int NOT NULL,

  ba_group_name varchar(255) default NULL,
  ba_group_description varchar(255) default NULL,
  visible enum('0', '1') NOT NULL default '1',

  PRIMARY KEY (id_ba_group)
);
CREATE SEQUENCE mod_bam_ba_groups_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_ba_groups_trigger
BEFORE INSERT ON mod_bam_ba_groups
FOR EACH ROW
BEGIN
  SELECT mod_bam_ba_groups_seq.nextval INTO :NEW.id_ba_group FROM dual;
END;
/
