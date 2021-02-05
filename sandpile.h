/****** definitions ******/
#define ARGS_SIZE 8
#define ARGS_DEPTH 2

void args_parse(int argc, char *argv[], int (*run_args)[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int (*topple_matrix)[9][3]);

void args_init(int (*run_args)[ARGS_SIZE][ARGS_DEPTH], int topple_matrix[9][3]);

int digit_count(int num);

void world_init(int argc, char *argv[], int run_args[ARGS_SIZE][ARGS_DEPTH], int ***world);

int world_update(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int ***world, int topple_matrix[9][3]);

int r_cell_topple(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int r_step, int ***world, int cell_y, int cell_x, int topple_matrix[9][3]);

int cell_inbounds(int world_height, int world_width, int cell_y, int cell_x);

void world_print(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int r_step, int **world);