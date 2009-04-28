/************************************************************************
 *
 * STMT.C - NDO Database Statements for optimized queries.
 * Copyright (c) 2009 Merethis
 *
 * Last Modified: 28-04-2009
 *
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#ifdef USE_MYSQL
# include <mysql.h>

/*
** Frees ressources occupied by a statement.
*/
void		ndo2db_stmt_delete(ndo2db_stmt* stmt)
{
  mysql_stmt_close(stmt);
  if (stmt->params)
    free(stmt->params);
  free(stmt);
  return ;
}

/*
** Execute a statement.
*/
int		ndo2db_stmt_execute(ndo2db_stmt* stmt)
{
  int		return_value;

  /* Try to execute the query with specified parameters. */
  if (mysql_stmt_bind_param(stmt->stmt, stmt->params)
      || mysql_stmt_execute(stmt))
    return_value = NDO_ERROR;
  else
    return_value = NDO_OK;

  /* Free parameters structures. */
  for (int i = 0; i < stmt->nb_params; i++)
    free(stmt->params[i].buffer);
  stmt->current_param = 0;
  return (return_value);
}

/*
** Create a new MySQL statement.
*/
ndo2db_stmt*	ndo2db_stmt_new(ndo2db_dbconninfo* dbconninfo)
{
  ndo2db_stmt*	stmt;

  if ((stmt = malloc(sizeof(*stmt))))
    {
      stmt->current_param = 0;
      stmt->nb_params = 0;
      stmt->params = NULL;
      if (!(stmt->stmt = mysql_stmt_init(&dbconninfo->mysql_conn)))
	{
	  free(stmt);
	  stmt = NULL;
	}
    }
  return (stmt);
}

/*
** Bind a parameter of type int to the query.
*/
int		ndo2db_stmt_param_int(ndo2db_stmt* stmt, int value)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = malloc(sizeof(value))))
    return (NDO_ERROR);
  *(int *)param->buffer = value;
  param->buffer_length = sizeof(value);
  param->buffer_type = MYSQL_TYPE_LONG;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter of type string to the query.
*/
int		ndo2db_stmt_param_string(ndo2db_stmt* stmt, const char* value)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = strdup(value)))
    return (NDO_ERROR);
  param->buffer_length = strlen(value);
  param->buffer_type = MYSQL_TYPE_STRING;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter of type time to the query.
*/
int		ndo2db_stmt_param_time(ndo2db_stmt* stmt, struct tm* tm)
{
  MYSQL_BIND*	param;
  MYSQL_TIME*	t;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(t = malloc(sizeof(*t))))
    return (NDO_ERROR);
  t->day = tm->tm_mday;
  t->hour = tm->tm_hour;
  t->minute = tm->tm_min;
  t->month = tm->tm_mon;
  t->neg = 0;
  t->second = tm->tm_sec;
  t->year = tm->tm_year;
  param->buffer = t;
  param->buffer_length = sizeof(*t);
  param->buffer_type = MYSQL_TYPE_DATETIME;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Parse a query for further execution.
*/
int		ndo2db_stmt_prepare(ndo2db_stmt* stmt, const char* query)
{
  char*		qmark;
  int		qmark_nb;

  /* Ask MySQL engine to parse the query. */
  if (mysql_stmt_prepare(stmt->stmt, query, strlen(query)))
    return (NDO_ERROR);

  /* Compute the number of question mark in the query. */
  qmark = strchr(query, '?');
  for (qmark_nb = 0; qmark; qmark_nb++)
    qmark = strchr(qmark + 1, '?');
  stmt->nb_params = qmark_nb;

  /* Allocate the number of MYSQL_BIND structures accordingly. */
  if (!(stmt->params = malloc(stmt->nb_params * sizeof(*stmt->params))))
    return (NDO_ERROR);
  stmt->current_param = 0;
  return (NDO_OK);
}

#endif /* !USE_MYSQL */
