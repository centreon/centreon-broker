#!/bin/sh

# Variables.
help=0
dbhost=
dbuser=
dbpassword=
dbname=
execute=0
clean=0
rebuild=0
tmpfile="/tmp/storage_reconciliate_metrics.tmp"

# Browse arguments.
while [ $# -ge 1 ] ; do
  if [ x"$1" = x"--help" ] ; then
    help=1
  elif [ x"$1" = x"--dbhost" ] ; then
    shift
    dbhost="$1"
  elif [ x"$1" = x"--dbuser" ] ; then
    shift
    dbuser="$1"
  elif [ x"$1" = x"--dbpassword" ] ; then
    shift
    dbpassword="$1"
  elif [ x"$1" = x"--dbname" ] ; then
    shift
    dbname="$1"
  elif [ x"$1" = x"--execute" ] ; then
    execute=1
  elif [ x"$1" = x"--clean" ] ; then
    clean=1
  elif [ x"$1" = x"--rebuild" ] ; then
    rebuild=1
  fi
  shift
done

# Help ?
if [ \( x"$help" = x1 \) \
  -o \( x"$dbhost" = x \) \
  -o \( x"$dbuser" = x \) \
  -o \( x"$dbpassword" = x \) \
  -o \( x"$dbname" = x \) ] ; then
  echo ""
  echo "Centreon Storage Reconciliation Utility"
  echo "---------------------------------------"
  echo ""
  echo "  --help                    Print this help."
  echo "  --dbhost <host>           DB host."
  echo "  --dbuser <user>           DB user."
  echo "  --dbpassword <password>   DB password."
  echo "  --dbname <name>           DB name."
  echo "  --execute                 Run SQL queries on the DB. If not specified"
  echo "                            queries are only printed."
  echo "  --clean                   Delete old entries from the metrics table."
  echo "  --rebuild                 Rebuild graphs of metrics that were"
  echo "                            reconciliated."
  echo ""
  exit 0
fi

# Get full metric list.
mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "SELECT index_id, metric_id, metric_name FROM rt_metrics ORDER BY index_id" "$dbname" > "$tmpfile"

# Browse all metrics.
lastindexid=
while read line ; do
  # Split components.
  indexid=`echo "$line" | cut -f 1`
  metricid=`echo "$line" | cut -f 2`
  metricname=`echo "$line" | cut -f 3-`

  # Modify metric name.
  oldmetricname=`echo "$metricname" | sed -e 's|/|#S#|g' -e 's|\\\\|#BS#|g' -e 's|%|#P#|g' -e 's|[^0-9_a-zA-Z#]|-|g'`

  # Process if metric name is modified.
  if [ x"$metricname" != x"$oldmetricname" ] ; then
    oldmetricid=`mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "SELECT metric_id FROM rt_metrics WHERE index_id=$indexid AND metric_name='$oldmetricname'" "$dbname"`
    if [ x"$oldmetricid" != x ] ; then
      echo "(index $indexid, metric $oldmetricid, name $oldmetricname)  =>  (index $indexid, metric $metricid, name $metricname)"

      # Move data entries to the new metric.
      if [ x"$execute" = x1 ] ; then
        mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "UPDATE log_data_bin SET id_metric=$metricid WHERE id_metric=$oldmetricid" "$dbname"
      else
        echo "  UPDATE log_data_bin SET id_metric=$metricid WHERE id_metric=$oldmetricid"
      fi

      # Remove the old metric.
      if [ x"$clean" = x1 ] ; then
        mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "DELETE FROM rt_metrics WHERE metric_id=$oldmetricid" "$dbname"
      else
        echo "  DELETE FROM rt_metrics WHERE metric_id=$oldmetricid"
      fi

      # Rebuild graphs.
      if [ x"$indexid" != x"$lastindexid" ] ; then
        if [ x"$rebuild" = x1 ] ; then
          mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "UPDATE rt_index_data SET must_be_rebuild=1 WHERE id=$indexid" "$dbname"
        else
          echo "UPDATE rt_index_data SET must_be_rebuild=1 WHERE id=$indexid"
        fi
        lastindexid="$indexid"
      fi
    fi
  fi

done < "$tmpfile"

# Removing temporary file.
rm -f "$tmpfile"
