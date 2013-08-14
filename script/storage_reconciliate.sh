#!/bin/sh

# Variables.
help=0
dbhost=
dbuser=
dbpassword=
dbname=
execute=0
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
  echo "Merethis Storage Reconciliation Utility"
  echo "---------------------------------------"
  echo ""
  echo "  --help                    Print this help."
  echo "  --dbhost <host>           DB host."
  echo "  --dbuser <user>           DB user."
  echo "  --dbpassword <password>   DB password."
  echo "  --dbname <name>           DB name."
  echo "  --execute                 Run SQL queries on the DB. If not specified"
  echo "                            queries are only printed."
  echo ""
  exit 0
fi

# Get full metric list.
mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "SELECT index_id, metric_id, metric_name FROM metrics" "$dbname" > "$tmpfile"

# Browse all metrics.
while read line ; do
  # Split components.
  indexid=`echo "$line" | cut -f 1`
  metricid=`echo "$line" | cut -f 2`
  metricname=`echo "$line" | cut -f 3-`

  # Modify metric name.
  oldmetricname=`echo "$metricname" | sed 's|/|#S#|g' | sed 's|#BS#|\|g' | sed 's|%P%|%|g'`

  # Process if metric name is modified.
  if [ x"$metricname" != x"$oldmetricname" ] ; then
    oldmetricid=`mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "SELECT metric_id FROM metrics WHERE index_id=$indexid AND metric_name='$oldmetricname'" "$dbname"`
    if [ x"$oldmetricid" != x ] ; then
      echo "(index $indexid, metric $oldmetricid, name $oldmetricname)  =>  (index $indexid, metric $metricid, name $metricname)"
      if [ x"$execute" = x1 ] ; then
        #mysql -B -N -h "$dbhost" -u "$dbuser" -p"$dbpassword" -e "UPDATE data_bin SET metric_id=$metricid WHERE metric_id=$oldmetricid" "$dbname"
      else
        echo "  UPDATE data_bin SET metric_id=$metricid WHERE metric_id=$oldmetricid"
      fi
    fi
  fi

done < "$tmpfile"

# Removing temporary file.
rm -f "$tmpfile"
