/************************************************************************
 *
 * STMT.H - NDO Database Statements Include File
 * Copyright (c) 2009 Merethis
 *
 * Last Modified: 28-04-2009
 *
 ************************************************************************/

#ifndef _NDO2DB_STMT_H
# define _NDO2DB_STMT_H

# include <time.h>
# include "ndo2db.h"
# ifdef USE_MYSQL
#  include <mysql.h>
# endif /* !USE_MYSQL */

/*
** Statement structure, as handled by convenient functions.
*/
typedef struct
{
  int		current_param;
  int		nb_params;
# ifdef USE_MYSQL
  MYSQL_BIND	*params;
  MYSQL_STMT	*stmt;
# endif /* !USE_MYSQL */
}		ndo2db_stmt;

/*
** Functions prototypes.
*/
void		ndo2db_stmt_delete(ndo2db_stmt* stmt);
int		ndo2db_stmt_execute(ndo2db_stmt* stmt, ...);
ndo2db_stmt*	ndo2db_stmt_new(ndo2db_dbconninfo* dbconninfo);
int		ndo2db_stmt_param_datetime(ndo2db_stmt* stmt);
int		ndo2db_stmt_param_double(ndo2db_stmt* stmt);
int		ndo2db_stmt_param_int(ndo2db_stmt* stmt);
int		ndo2db_stmt_param_smallint(ndo2db_stmt* stmt);
int		ndo2db_stmt_param_string(ndo2db_stmt* stmt);
int		ndo2db_stmt_prepare(ndo2db_stmt* stmt,
				    const char* query,
				    int nb_col);

#endif /* !_NDO2DB_STMT_H */
