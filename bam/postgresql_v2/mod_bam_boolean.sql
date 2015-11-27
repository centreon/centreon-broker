--
-- BAM boolean expressions.
--
CREATE TABLE mod_bam_boolean (
  boolean_id serial,
  name varchar(255) NOT NULL,

  expression text NOT NULL,
  bool_state boolean NOT NULL default 1,
  comments text default NULL,
  activate boolean NOT NULL default 0,

  PRIMARY KEY (boolean_id)
);
