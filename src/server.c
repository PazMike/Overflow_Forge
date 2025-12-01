/**
 * server.c
 * gcc -lulfius -lmysqlclient server.c
 * Written by Judah Sotomayor
 */
#include <stdio.h>
#include <ulfius.h>
#include <mysql/mysql.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/mman.h>

#define PORT 8080

int staff_div(char* name, char* title, char* years, char* bio, char** result) {
  int space_needed = asprintf(result, "<div>"
       "<h2>%s</h2>"
       "<p>%s</p>"
       "<div class=\"years\">%s years of experience</div>"
       "<div class=\"bio\"> %s</div>"
       "</div>", name, title, years, bio);
       if(space_needed <= 0) {
         space_needed = asprintf(result, "<div>No data</div>");
       }
       return space_needed;
}

int service_div(char* name, char* waitTime, char** result) {
  int space_needed = asprintf(result, "<div>"
                            "<h2>%s</h2>"
                            "<p>Average wait: %s</p>"
                            "</div>", name, waitTime);
  if(space_needed <= 0) {
    space_needed = asprintf(result, "<div>No data</div>");
  }
  return space_needed;
}

int loc_div(char* code, char* name, char* type, char* address, char* city, char* state, char* zip, char* phone, char* opened, char** result) {
  int space_needed = asprintf(result,
                            "<article>"
                            "<header>%s</header>"
                            "<ul>"
                            "<li><strong>Code:</strong> %s</li>"
                            "<li><strong>Address:</strong> %s</li>"
                            "<li><strong>City:</strong> %s, %s %s</li>"
                            "<li><strong>Type:</strong> %s</li>"
                            "<li><strong>Phone:</strong> %s</li>"
                            "</ul>"
                            "<footer>"
                            "<small>Opened: %s</small>"
                            "</footer>"
                            "</article>",
                            name, code, address, city, state, zip, type, phone, opened);
  if(space_needed <= 0) {
    space_needed = asprintf(result, "<article>No data</article>");
  }
  return space_needed;
}


/**
 * Callback function for the web application on /helloworld url call
 */
int callback_root (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int fd = open("index.html", O_RDONLY);
  int len = lseek(fd, 0, SEEK_END);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  ulfius_set_string_body_response(response, 200, (char*)data);
  return U_CALLBACK_CONTINUE;
}

int callback_practitioners (const struct _u_request * request, struct _u_response * response, void * user_data) {
  MYSQL *conn = (MYSQL*)user_data;
  MYSQL_RES *res = NULL;
  MYSQL_ROW row;
  char* buffer = NULL;
  char query[100];
  sprintf(query, "SELECT * FROM practitioners WHERE full_name LIKE '%%%s%%'", u_map_get(request->map_url, "name"));
  if (mysql_query(conn, query)) {
    fprintf(stderr, "%s\n", mysql_error(conn));

    return U_CALLBACK_ERROR;
  }


  res = mysql_store_result(conn);

  int num_rows = mysql_num_rows(res);

  if(num_rows > 0) {

    size_t total_length = 0;
    char* results[num_rows];

    for(int i = 0; i < num_rows; i++) {
      row = mysql_fetch_row(res);
      total_length += staff_div(row[1], row[2], row[4], row[5], &results[i]);
    }

    mysql_free_result(res);

    buffer = calloc(total_length, sizeof(char));
    for(int i = 0; i < num_rows; i++) {
      strcat(buffer, results[i]);
      free(results[i]);
    }
  } else {
    buffer = calloc(50, sizeof(char));
    strcat(buffer, "<div><h1>No results!</h></div>");
  }


  ulfius_set_string_body_response(response, 200, buffer);
  return U_CALLBACK_CONTINUE;

}
int callback_services (const struct _u_request * request, struct _u_response * response, void * user_data) {
  MYSQL *conn = (MYSQL*)user_data;
  MYSQL_RES *res = NULL;
  MYSQL_ROW row;
  char* buffer = NULL;
  char query[200];
  sprintf(query, "SELECT * FROM services WHERE service_name LIKE '%%%s%%'", u_map_get(request->map_url, "name"));
  if (mysql_query(conn, query)) {
    fprintf(stderr, "%s\n", mysql_error(conn));

    return U_CALLBACK_ERROR;

  }

  res = mysql_store_result(conn);

  int num_rows = mysql_num_rows(res);

  if(num_rows > 0) {

    size_t total_length = 0;
    char* results[num_rows];

    for(int i = 0; i < num_rows; i++) {
      row = mysql_fetch_row(res);
      total_length += service_div(row[1], row[2], &results[i]);
    }

    mysql_free_result(res);

    buffer = calloc(total_length, sizeof(char));
    for(int i = 0; i < num_rows; i++) {
      strcat(buffer, results[i]);
      free(results[i]);
    }
  } else {
    buffer = calloc(50, sizeof(char));
    strcat(buffer, "<div><h1>No results!</h></div>");
  }


  ulfius_set_string_body_response(response, 200, buffer);
  return U_CALLBACK_CONTINUE;

}
int callback_locations (const struct _u_request * request, struct _u_response * response, void * user_data) {
  MYSQL *conn = (MYSQL*)user_data;
  MYSQL_RES *res = NULL;
  MYSQL_ROW row;
  char* buffer = NULL;
  char query[950];

  const char* lq = u_map_get(request->map_url, "name");

  sprintf(query, "SELECT * FROM locations "
          "WHERE location_code LIKE '%%%s%%' "
          "OR location_name LIKE '%%%s%%' "
          "OR location_type LIKE '%%%s%%' "
          "OR zip_code LIKE '%%%s%%' "
          "OR phone LIKE '%%%s%%' "
          "OR address LIKE '%%%s%%' "
          "OR city LIKE '%%%s%%' ", lq, lq, lq, lq, lq, lq, lq);

  if (mysql_query(conn, query)) {
    fprintf(stderr, "%s\n", mysql_error(conn));
    return U_CALLBACK_ERROR;
  }


  res = mysql_store_result(conn);

  int num_rows = mysql_num_rows(res);

  if(num_rows > 0) {
    size_t total_length = 0;
    char* results[num_rows];

    for(int i = 0; i < num_rows; i++) {
      row = mysql_fetch_row(res);
      total_length += loc_div(row[1], row[2], row[4], row[5], row[6], row[7], row[8], row[9], row[11], &results[i]);
    }

    mysql_free_result(res);

    buffer = calloc(total_length, sizeof(char));
    for(int i = 0; i < num_rows; i++) {
      strcat(buffer, results[i]);
      free(results[i]);
    }
  } else {
    buffer = calloc(50, sizeof(char));
    strcat(buffer, "<div><h1>No results!</h></div>");
  }

  ulfius_set_string_body_response(response, 200, buffer);
  return U_CALLBACK_CONTINUE;

}

// \x00\x40\x13\x4e
/**
 * main function
 */
int main(void) {

  MYSQL *conn;
  char *server = "127.0.0.1";
  char *user = "root";
  char *password = "Chiapet1!";
  char *database = "bfss";

  conn = mysql_init(NULL);

  if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
    fprintf(stderr, "Cannot connect to database %s at %s using %s:%s", database, server, user, password);
    return(1);
  }
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/", NULL, 0, &callback_root, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", "/search/practitioners", NULL, 0, &callback_practitioners, (void*)conn);
  ulfius_add_endpoint_by_val(&instance, "GET", "/search/services", NULL, 0, &callback_services, (void*)conn);
  ulfius_add_endpoint_by_val(&instance, "GET", "/search/locations", NULL, 0, &callback_locations, (void*)conn);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    // Wait for the user to press <enter> on the console to quit the application
    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
}
