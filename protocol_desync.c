#include <stdio.h>
#include <stdlib.h>

#include <libpq-fe.h>

static void exit_nicely(PGconn *conn) {
  PQfinish(conn);
  exit(1);
}

static void send_a_query(PGconn *conn) {
  PGresult *res;

  if (!PQsendQueryParams(conn, "SELECT 1", 0, NULL, NULL, NULL, NULL, 1)) {
    exit_nicely(conn);
  }

  if (!PQpipelineSync(conn)) {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  if (!PQpipelineSync(conn)) {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  int ready_for_query_count = 0;
  int fatal_error_count = 0;

  int loop_iterations_count = 0;
  while (PQstatus(conn) != CONNECTION_BAD) {
    res = PQgetResult(conn);
    if (res != NULL) {
      if (PQresultStatus(res) == PGRES_PIPELINE_SYNC) {
        ++ready_for_query_count;
      }
      if (PQresultStatus(res) == PGRES_FATAL_ERROR) {
        ++fatal_error_count;
      }
    }

    PQclear(res);

    if (++loop_iterations_count > 1000) {
      fprintf(stderr, "Finished consuming results\n");
      if (PQstatus(conn) == CONNECTION_OK) {
        fprintf(stderr, "Connection is in CONNECTION_OK status\n");
      }
      break;
    }
  }

  fprintf(stderr,
          "libpq send 2 Syncs, "
          "got %i PGRES_FATAL_ERRORs and %i PGRES_PIPELINE_SYNCs\n",
          fatal_error_count, ready_for_query_count);
  fprintf(stderr, "How comes we are finishing without reading second Z(ReadyForQuery)?\n");
  fflush(stderr);
}

int main(int argc, char **argv) {
  const char *conninfo;
  PGconn *conn;
  int nFields;
  int i, j;

  /*
   * If the user supplies a parameter on the command line, use it as the
   * conninfo string; otherwise default to setting dbname=postgres and using
   * environment variables or defaults for all other connection parameters.
   */
  if (argc > 1)
    conninfo = argv[1];
  else
    conninfo = "dbname = postgres";

  /* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK) {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  if (!PQenterPipelineMode(conn)) {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  send_a_query(conn);

  PQfinish(conn);
  fprintf(stderr, "Finishing, unexpectedly\n");

  return 0;
}
