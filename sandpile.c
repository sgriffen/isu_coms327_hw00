/****** include std libs *******/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/****** include custom libs ******/
#include "sandpile.h"

/****** global vars ******/
char *ansi_colors[] = {
        "\x1b[97m", //white
        "\x1b[93m", //bright yellow
        "\x1b[96m", //bright cyan
        "\x1b[33m", //dark yellow
        "\x1b[92m", //bright green
        "\x1b[32m", //dark green
        "\x1b[31m", //dark red
        "\x1b[95m", //bright purple
        "\x1b[35m", //purple
};

int main(int argc, char *argv[]) {

    /* init variables */
	unsigned long long frame_count = 0;
    int topple_matrix[9][3] = {
            { 1,  0,  0 },
            { 1, -1,  0 },
            { 1, -1,  1 },
            { 1,  0,  1 },
            { 1,  1,  1 },
            { 1,  1,  0 },
            { 1,  1, -1 },
            { 1,  0, -1 },
            { 1, -1, -1 }
    };
    int run_args[ARGS_SIZE][ARGS_DEPTH] = {
            {5, 0}, //default framerate
            {23, 23}, //default map size
            {-1, -1}, //default fall location
            {1, 0}, //default step value
            {8, 2}, //default fall height and number of digits to print
            {-1, 0}, //default color scheme
            {0, 0}, //deep debug
			{-1, 0} //frame stop
    };
    char run_args_char[] = { '#', '0' }; //default chars for a sink and an empty pile
//	FILE* pgm_imgs[9];
    int **world = NULL;

    /* get custom cmd args if applicable */
    if (argc > 1) { args_parse(argc, argv, &run_args, run_args_char, &topple_matrix); } //parse arguments

    args_init(&run_args, topple_matrix); //init unset and calibrate runtime arguments

//	if (run_args[4][0] == 2) {
//
//
//	}

    /* initialize world */
    world_init(argc, argv, run_args, &world); //init world
    world_print(run_args, run_args_char, -1, world); //print world plane

    /* simulation loop */
    while(1) {
		
		frame_count++;
		
        /* control fps */
        if (run_args[0][0] < 1) { //fps is in debug mode
			
            char temp;
            scanf("%c", &temp);
			
			printf("Frame #[%llu]", frame_count);
        }
		else if (run_args[7][0] > -1) { //pause-at frame(s) defined
			
			if (run_args[7][1] && frame_count > 1 && frame_count % run_args[7][0] == 0) {
				
				char temp;
				scanf("%c", &temp);
			}
			else if (frame_count == run_args[7][0]) {
				
				char temp;
				scanf("%c", &temp);
			}
			else { usleep(1000000 / run_args[0][0]); } //fps is in normal mode
		}
        else { usleep(1000000 / run_args[0][0]); } //fps is in normal mode

        /* run simulation cycle */
        world_update(run_args, run_args_char, &world, topple_matrix); //update world plane
        world_print(run_args, run_args_char, -1, world); //print world plane if not in deep-debug mode
    }

    free(world);

    return 0;
}

void args_parse(int argc, char *argv[], int (*run_args)[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int (*topple_matrix)[9][3]) {

    int i = 1;

    if (!strcmp(argv[i], "--fps")) { //fps arg

        if (!strcmp(argv[i + 1], "deep-debug")) { //get deep debug mode (shows recursion steps and fps follows rate of enter key presses)

            (*run_args)[0][0] = -1;
            (*run_args)[6][0] = 1;
        }
        else if (!strcmp(argv[i + 1], "debug")) { (*run_args)[0][0] = -1; } //get normal debug mode (fps follows rate of enter key presses)
        else if (atoi(argv[i + 1]) > 0) { (*run_args)[0][0] = atoi(argv[i + 1]); }

        i += 2;
    }

    while (i < argc && argv[i][0] != '-') { i++; } //skip through initial pile heights/sinks

    while (i < argc) { //check that custom commands exist

        if (!strcmp(argv[i], "--size")) { //get world size if applicable

            if (atoi(argv[i + 1]) > -1) { (*run_args)[1][0] = atoi(argv[i + 1]); }
            if (atoi(argv[i + 2]) > -1) { (*run_args)[1][1] = atoi(argv[i + 2]); }

            i += 3;
        }
        else if (!strcmp(argv[i], "--step-at")) { //get step location if applicable

            if (!strcmp(argv[i + 1], "rand")) 											{ (*run_args)[2][0] = -2; } //using -1 to denote random cell_y fall location desired each update loop
            else if (atoi(argv[i + 1]) > -1 && atoi(argv[i + 2]) < (*run_args)[1][0]) 	{ (*run_args)[2][0] = atoi(argv[i + 1]); } //value must be within size of world
            if (!strcmp(argv[i + 2], "rand")) 											{ (*run_args)[2][1] = -2; } //using -1 to denote random cell_x fall location desired each update loop
            else if (atoi(argv[i + 2]) > -1 && atoi(argv[i + 2]) < (*run_args)[1][1]) 	{ (*run_args)[2][1] = atoi(argv[i + 2]); } //value must be within size of world

            i += 3;
        }
        else if (!strcmp(argv[i], "--step-size")) { //get step amount if applicable

            if (atoi(argv[i + 1]) > 0) { (*run_args)[3][0] = atoi(argv[i + 1]); }

            i += 2;
        }
        else if (!strcmp(argv[i], "--topple-height")) { //get max height of cell if applicable

            (*run_args)[4][0] = atoi(argv[i + 1]);

            i += 2;
        }
        else if (!strcmp(argv[i], "--topple-matrix")) { //get topple zone matrix if applicable

            int j = i + 1;

            while (j < argc && argv[j][0] != '-') {

                (*topple_matrix)[(j - i)][0] = atoi(argv[j]);

                j++;
            }

            i = j;
        }
        else if (!strcmp(argv[i], "--char")) { //get desired chars for 'sink' and 'zero' if applicable

            run_args_char[0] = argv[i + 1][0];
            run_args_char[1] = argv[i + 2][0];

            i += 3;
        }
        else if (!strcmp(argv[i], "--color")) { //get color mode if applicable

            (*run_args)[5][0] = 1;

            i++;
        }
		else if (!strcmp(argv[i], "--pause-at")) {
			
			if (!strcmp(argv[i + 1], "multi")) {
				
				(*run_args)[7][1] = 1;
				i++;
			}
			if (atoi(argv[i + 1]) > 0) { (*run_args)[7][0] = atoi(argv[i + 1]); }
			else {
				
				(*run_args)[7][0] = -1;
				(*run_args)[7][1] = 0;
			}
			
			i += 2;
		}
        else { i++; }
    }

    return;
}

void args_init(int (*run_args)[ARGS_SIZE][ARGS_DEPTH], int topple_matrix[9][3]) {
	
	int num_topple_zones = 0;
	int i;
	
    /* init unset runtime arguments */
    if ((*run_args)[2][0] == -1) { //default fall location to center of map

        if ((*run_args)[2][0] % 2 == 1) { (*run_args)[2][0] = ((*run_args)[1][0] / 2) - 1; }
        else { (*run_args)[2][0] = ((*run_args)[1][0] / 2); }

    }
    if ((*run_args)[2][1] == -1) { //default fall location to center of map

        if ((*run_args)[2][1] % 2 == 1) { (*run_args)[2][1] = ((*run_args)[1][1] / 2) - 1; }
        else { (*run_args)[2][1] = ((*run_args)[1][1] / 2); }
    }
	
	/* ensure topple height is never less than the number of cells you can topple to. If this check is not made, program will enver an infinite recursion loop while toppling and will seg fault */
	for (i = 1; i < 9; i++) {
		
		if (topple_matrix[i][0]) { num_topple_zones++; }
	}
	if ((*run_args)[4][0] < num_topple_zones) { (*run_args)[4][0] = num_topple_zones; }
    (*run_args)[4][1] = digit_count((*run_args)[4][0]) + 1; //get number of digits in topple height (used for print formatting later)
	
	/* ensure world size is strictly positive */
	if ((*run_args)[1][0] < 1) { (*run_args)[1][0] = 23; }
	if ((*run_args)[1][1] < 1) { (*run_args)[1][1] = 23; }
	
    return;
}

int digit_count(int num) {

    int count = 0;

    do {

        count++;
        num /= 10;
    } while(num);

    return count;
}

void world_init(int argc, char *argv[], int run_args[ARGS_SIZE][ARGS_DEPTH], int ***world) {

    int i = 0, j = 0;

    /* allocate memory for world and initialize values */
    *world = (int**)malloc(sizeof(int*) * run_args[1][0]);
    for(i = 0; i < run_args[1][0]; i++) {

        (*world)[i] = (int*)malloc(sizeof(int) * run_args[1][1]);
        for (j = 0; j < run_args[1][1]; j++) { (*world)[i][j] = 0; }
    }

    /* populate world with initial values if applicable */
    if (argc > 1) {

        int i;
        int i_start = 1;

        if (!strcmp(argv[1], "--fps")) { i_start += 2; }

        for (i = i_start; i < argc; i += 3) { //rest of args are in tuples, so iterate by 3

            if (argv[i][0] == '-') { break; } //tuple assignments are complete, rest of args are optional flags

            int cell_y = atoi(argv[i]); //get cell_y cell
            int cell_x = atoi(argv[i + 1]); //get cell_x cell
            int z = atoi(argv[i + 2]); //get height at cell (cell_y, cell_x)

            if (cell_y < run_args[1][0] && cell_x < run_args[1][1]) { //check that specified cell_y and cell_x coordinates are within the world plane

                if (z < 0 && (cell_y != run_args[2][1] || cell_x != run_args[2][0])) { (*world)[cell_y][cell_x] = -1; } //create sink if height is less than 0 and cell is not one where falling happens
                if (z > -1) { //init starting height

                    if (z < (run_args[4][0] + 1))   { (*world)[cell_y][cell_x] = z; }
                    else                            { (*world)[cell_y][cell_x] = run_args[4][0]; } //max out starting height at 8
                }
            }
        }
    }

    return;
}

int world_update(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int ***world, int topple_matrix[9][3]) {

    int cell_y, cell_x;
    if (run_args[2][0] < 0) { cell_y = rand() % run_args[1][0]; }
    else 					{ cell_y = run_args[2][0]; }
    if (run_args[2][1] < 0) { cell_x = rand() % run_args[1][1]; }
    else 					{ cell_x = run_args[2][1]; }
	
	while ((*world)[cell_y][cell_x] < 0) { //if random location picked is a sink, cannot step there
		
		if (run_args[2][0] < 0) { cell_y = rand() % run_args[1][0]; }
		if (run_args[2][1] < 0) { cell_x = rand() % run_args[1][1]; }
	}
	
    (*world)[cell_y][cell_x] += run_args[3][0];

    return r_cell_topple(run_args, run_args_char, -1, world, cell_y, cell_x, topple_matrix);
}

int r_cell_topple(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int r_step, int ***world, int cell_y, int cell_x, int topple_matrix[9][3]) {

    int topple_amount = 0;
    int i;

    if (!cell_inbounds(run_args[1][0], run_args[1][1], cell_y, cell_x)) { return 0; } //cell is outside the world, so do nothing
    if ((*world)[cell_y][cell_x] < 0) { return 0; } //cell is a sink, so do nothing

    if ((*world)[cell_y][cell_x] > run_args[4][0]) {

        if (run_args[6][0]) {

            char temp;

            world_print(run_args, run_args_char, r_step, *world);
            scanf("%c", &temp);
        }

        /* get amount to topple by. depends on how many cells around current can be toppled to */
        for (i = 1; i < 9; i++) {

            if (topple_matrix[i][0]) { topple_amount++; } //if cell can be toppled to, increase topple amount
        }

        /* topple spot by topple amount */
        (*world)[cell_y][cell_x] = (*world)[cell_y][cell_x] - topple_amount;

        /* Increase topple tiles defined in topple_matrix by step size */
        for (i = 1; i < 9; i++) {

            if (topple_matrix[i][0] && cell_inbounds(run_args[1][0], run_args[1][1], cell_y+topple_matrix[i][1], cell_x+topple_matrix[i][2])) { //if cell can be toppled to and is in the world, increase cell value by 1 and topple it if applicable

                if ((*world)[cell_y+topple_matrix[i][1]][cell_x+topple_matrix[i][2]] > -1) { (*world)[cell_y+topple_matrix[i][1]][cell_x+topple_matrix[i][2]] += 1; } //if cell is not a sink, add 1 to it's value
                r_cell_topple(run_args, run_args_char, r_step+1, world, cell_y+topple_matrix[i][1], cell_x+topple_matrix[i][2], topple_matrix); //if can, topple
            }
        }
    }

    return 1;
}

int cell_inbounds(int world_height, int world_width, int cell_y, int cell_x) {

    if (cell_y < 0 || cell_y >= world_height || cell_x < 0 || cell_x >= world_width) { return 0; }

    return 1;
}

void world_print(int run_args[ARGS_SIZE][ARGS_DEPTH], char run_args_char[], int r_step, int **world) {

    int size_y = run_args[1][0];
    int size_x = run_args[1][1];
    int i = 0, j = 0;
	
	//        printf("\x1b[2J"); //clear console
    printf("\n"); //print newline
	
    if (r_step > -1) { printf("recursion step: [%d]\n", r_step); }

    for (i = 0; i < size_y; i++) {
        for (j = 0; j < size_x; j++) {

            int color_index = 0;

            if (world[i][j] < 0) {

                if (run_args[5][0] == 2) { printf("%s%*c%s", ansi_colors[color_index], run_args[4][1], '0', "\x1b[0m"); }
                printf("%s%*c%s", ansi_colors[color_index], run_args[4][1], run_args_char[0], "\x1b[0m");
            }
            else if (world[i][j] == 0) { printf("%s%*c%s", ansi_colors[color_index], run_args[4][1], run_args_char[1], "\x1b[0m"); }
            else {

                if (run_args[5][0] == 1 && world[i][j] < 9) { color_index += (world[i][j]); }
                if (run_args[5][0] == 1) { printf("%s%*d%s", ansi_colors[color_index], run_args[4][1], world[i][j], "\x1b[0m"); }
                else { printf("%*d", run_args[4][1], world[i][j]); }
            }
        }
        printf("\n");
    }

    return;
}