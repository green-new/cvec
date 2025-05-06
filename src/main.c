
void init(void) {
  printf("Initializing program");
}

void run(void) {
  printf("Running main loop")
}

void exit(void) {
  printf("Closing the application");
  exit(0);
}

int main(int argc, char** argv) {
  printf("Hello world");

  init();

  run();

  exit();

  return 0;
}