/************************************************************************
 *
 * STMT.C - NDO Database Statements for optimized queries.
 * Copyright (c) 2009 Merethis
 *
 * Last Modified: 28-04-2009
 *
 ************************************************************************/

#include <mysql.h>
#include <stdlib.h>
#include <time.h>
#include "../include/common.h"
#include "../include/stmt.h"
#ifdef USE_MYSQL
# include <mysql.h>

/*
** Free ressources occupied by arguments.
*/
static void	ndo2db_stmt_params_free(ndo2db_stmt* stmt)
{
  if (stmt->params)
    {
      for (int i = 0; i < stmt->nb_params; i++)
	if (stmt->params[i].buffer
	    && stmt->params[i].buffer_type != MYSQL_TYPE_STRING)
	  free(stmt->params[i].buffer);
      free(stmt->params);
      stmt->params = NULL;
    }
  return ;
}

/*
** Free ressources occupied by a statement.
*/
void		ndo2db_stmt_delete(ndo2db_stmt* stmt)
{
  mysql_stmt_close(stmt->stmt);
  if (stmt->params)
    {
      ndo2db_stmt_params_free(stmt);
    }
  free(stmt);
  return ;
}

/*
** Execute a statement.
*/
int		ndo2db_stmt_execute(ndo2db_stmt* stmt, ...)
{
  int		return_value;
  va_list	args;

  /* Fetch query arguments. */
  va_start(args, stmt);
  for (int i = 0; i < stmt->nb_params; i++)
    switch (stmt->params[i].buffer_type)
      {
	MYSQL_TIME*	mt;
	struct tm*	tm;

	/* DATETIME argument. */
      case MYSQL_TYPE_DATETIME:
	mt = stmt->params[i].buffer;
	tm = va_arg(args, struct tm*);
	mt->day = tm->tm_mday;
	mt->hour = tm->tm_hour;
	mt->minute = tm->tm_min;
	mt->month = tm->tm_mon;
	mt->neg = 0;
	mt->second = tm->tm_sec;
	mt->second_part = 0;
	mt->year = tm->tm_year + 1900;
	break ;

	/* DOUBLE argument. */
      case MYSQL_TYPE_DOUBLE:
	*(double*)stmt->params[i].buffer = va_arg(args, double);
	break ;

	/* INT argument. */
      case MYSQL_TYPE_LONG:
	*(int*)stmt->params[i].buffer = va_arg(args, int);
	break ;

	/* SMALLINT argument. */
      case MYSQL_TYPE_SHORT:
	*(short int*)stmt->params[i].buffer = va_arg(args, int);
	break ;

	/* STRING argument. */
      case MYSQL_TYPE_STRING:
	stmt->params[i].buffer = va_arg(args, char*);
	stmt->params[i].buffer_length = strlen(stmt->params[i].buffer);
	break ;
       
	/* Unhandled argument type. */
      default:
	break ;
      }
  va_end(args);

  /* Try to execute the query with specified parameters. */
  if (mysql_stmt_bind_param(stmt->stmt, stmt->params)
      || mysql_stmt_execute(stmt->stmt))
    return_value = NDO_ERROR;
  else
    return_value = NDO_OK;
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
** Bind a parameter to SQL type DATETIME (struct tm).
*/
int		ndo2db_stmt_param_datetime(ndo2db_stmt* stmt)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = malloc(sizeof(MYSQL_TIME))))
    return (NDO_ERROR);
  param->buffer_length = sizeof(MYSQL_TIME);
  param->buffer_type = MYSQL_TYPE_DATETIME;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter to SQL type DOUBLE (double).
*/
int		ndo2db_stmt_param_double(ndo2db_stmt* stmt)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = malloc(sizeof(double))))
    return (NDO_ERROR);
  param->buffer_length = sizeof(double);
  param->buffer_type = MYSQL_TYPE_DOUBLE;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter to SQL type INT (int).
*/
int		ndo2db_stmt_param_int(ndo2db_stmt* stmt)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = malloc(sizeof(int))))
    return (NDO_ERROR);
  param->buffer_length = sizeof(int);
  param->buffer_type = MYSQL_TYPE_LONG;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter to SQL type SMALLINT (short).
*/
int		ndo2db_stmt_param_smallint(ndo2db_stmt* stmt)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  if (!(param->buffer = malloc(sizeof(short))))
    return (NDO_ERROR);
  param->buffer_length = sizeof(short);
  param->buffer_type = MYSQL_TYPE_SHORT;
  param->error = NULL;
  param->is_null = NULL;
  param->is_unsigned = 0;
  param->length = &param->buffer_length;
  stmt->current_param++;
  return (NDO_OK);
}

/*
** Bind a parameter to SQL type TEXT, CHAR or VARCHAR (const char *).
*/
int		ndo2db_stmt_param_string(ndo2db_stmt* stmt)
{
  MYSQL_BIND*	param;

  param = stmt->params + stmt->current_param;
  memset(param, 0, sizeof(*param));
  param->buffer = NULL;
  param->buffer_length = 0;
  param->buffer_type = MYSQL_TYPE_STRING;
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
int		ndo2db_stmt_prepare(ndo2db_stmt* stmt,
				    const char* query,
				    int nb_col)
{
  /* Ask MySQL engine to parse the query. */
  if (mysql_stmt_prepare(stmt->stmt, query, strlen(query)))
    return (NDO_ERROR);

  /* Allocate MYSQL_BIND structures. */
  ndo2db_stmt_params_free(stmt);
  if (!(stmt->params = malloc(nb_col * sizeof(*stmt->params))))
    return (NDO_ERROR);
  stmt->nb_params = nb_col;
  stmt->current_param = 0;
  return (NDO_OK);
}

#endif /* !USE_MYSQL */
