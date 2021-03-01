#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gpiod.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <string.h>


int cyclic = 2;

void function_pt(void *ptr, size_t size, size_t nmemb, void *stream){
    char *tmp = malloc(sizeof(char) * (nmemb + 1));
    memset(tmp, 0, sizeof(char) * (nmemb + 1));
    for (int i = 0; i < nmemb; i++)
      tmp[i] = ((char *)ptr)[i];
    cyclic = atoi(tmp);
    fprintf(stdout, "cyclic: %d\n", cyclic);
}

void call_get(void)
{
  CURL *curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/get");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
}

int main(int ac, char **av)
{
  struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
  struct gpiod_line *line = gpiod_chip_get_line(chip, 17);
  int ret = gpiod_line_request_output(line, "Consumer", 0);

  call_get();


  ret = gpiod_line_set_value(line, false);

  int implicit_cyclic = 1000000/50;
  int implicit_cyclic_on = (implicit_cyclic * cyclic) / 100;
  int implicit_cyclic_off = implicit_cyclic - implicit_cyclic_on;

  int tmp = 0;

  fprintf(stdout, "Starting motor control with a cyclic of %d\n", cyclic);
  while (true) {
    if (tmp >= 100) {
      call_get();
      implicit_cyclic_on = (implicit_cyclic * cyclic) / 100;
      implicit_cyclic_off = implicit_cyclic - implicit_cyclic_on;
      tmp = 0;
    }
    ret = gpiod_line_set_value(line, true);
    usleep(implicit_cyclic_on);
    ret = gpiod_line_set_value(line, false);
    usleep(implicit_cyclic_off);
    tmp += 1;
  }
  return (0);
}
