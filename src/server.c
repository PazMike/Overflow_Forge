/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <ulfius.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/mman.h>

#define PORT 8080

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

int callback_submit_p (const struct _u_request * request, struct _u_response * response, void * user_data) {
    printf("POST parameter id: %s\n", u_map_get(request->map_post_body, "name"));

    const char * str = u_map_get(request->map_post_body, "name");
    char buffer [100];
    int result = sprintf(buffer, "<div> result %s </div>", str);
    ulfius_set_string_body_response(response, 200, buffer);
    return U_CALLBACK_CONTINUE;
}
/**
 * main function
 */
int main(void) {
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/", NULL, 0, &callback_root, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", "/submit", NULL, 0, &callback_submit_p, NULL);

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
