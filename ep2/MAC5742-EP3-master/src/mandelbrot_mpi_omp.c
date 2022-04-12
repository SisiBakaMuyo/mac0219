#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>

#define MASTER 0

double c_x_min;
double c_x_max;
double c_y_min;
double c_y_max;

int i_x_max;
int i_y_max;

int nthreads;
int current_i_y;
int current_rec;
omp_lock_t stacklock;

int iteration_max = 200;

double pixel_width;
double pixel_height;

int image_size;
int image_buffer_size;
unsigned char **image_buffer;

int gradient_size = 16;
int colors[17][3] = {
    {66, 30, 15},
    {25, 7, 26},
    {9, 1, 47},
    {4, 4, 73},
    {0, 7, 100},
    {12, 44, 138},
    {24, 82, 177},
    {57, 125, 209},
    {134, 181, 229},
    {211, 236, 248},
    {241, 233, 191},
    {248, 201, 95},
    {255, 170, 0},
    {204, 128, 0},
    {153, 87, 0},
    {106, 52, 3},
    {0, 0, 0},
};

void allocate_image_buffer() {
    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);

    for (int i = 0; i < image_buffer_size; i++) {
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    };

};

void init(int argc, char *argv[]){

    if(argc < 6){
        printf("usage: ./mandelbrot_seq c_x_min c_x_max c_y_min c_y_max image_size\n");
        printf("examples with image_size = 11500:\n");
        printf("    Full Picture:         ./mandelbrot_seq -2.5 1.5 -2.0 2.0 11500\n");
        printf("    Seahorse Valley:      ./mandelbrot_seq -0.8 -0.7 0.05 0.15 11500\n");
        printf("    Elephant Valley:      ./mandelbrot_seq 0.175 0.375 -0.1 0.1 11500\n");
        printf("    Triple Spiral Valley: ./mandelbrot_seq -0.188 -0.012 0.554 0.754 11500\n");
        exit(0);
    }
    else{
        sscanf(argv[1], "%lf", &c_x_min);
        sscanf(argv[2], "%lf", &c_x_max);
        sscanf(argv[3], "%lf", &c_y_min);
        sscanf(argv[4], "%lf", &c_y_max);
        sscanf(argv[5], "%d", &image_size);

        i_x_max           = image_size;
        i_y_max           = image_size;
        image_buffer_size = image_size * image_size;

        pixel_width       = (c_x_max - c_x_min) / i_x_max;
        pixel_height      = (c_y_max - c_y_min) / i_y_max;
    };

};

void update_rgb_buffer(int iteration, int x, int y){

    int color;

    if(iteration == iteration_max){
        image_buffer[(i_y_max * y) + x][0] = colors[gradient_size][0];
        image_buffer[(i_y_max * y) + x][1] = colors[gradient_size][1];
        image_buffer[(i_y_max * y) + x][2] = colors[gradient_size][2];
    }
    else{
        color = iteration % gradient_size;

        image_buffer[(i_y_max * y) + x][0] = colors[color][0];
        image_buffer[(i_y_max * y) + x][1] = colors[color][1];
        image_buffer[(i_y_max * y) + x][2] = colors[color][2];
    };

};

void write_to_file() {
    FILE * file;
    char * filename               = "output.ppm";
    char * comment                = "# ";

    int max_color_component_value = 255;

    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1 , 3, file);
    };

    fclose(file);
};

void master_mandelbrot(int ntasks, int taskid) {

    int tid;

    #pragma omp parallel private(nthreads, tid)
    {

        tid = omp_get_thread_num();

        int iteration;

        double c_x;
        double c_y;
        double z_x;
        double z_y;
        double z_x_squared;
        double z_y_squared;
        double escape_radius_squared = 4;

        int i_y;
        int i_x;
        

        omp_set_lock(&stacklock);
        i_y = current_i_y;
        current_i_y+=ntasks;
        omp_unset_lock(&stacklock);

        while (i_y < image_size) {

            c_y = c_y_min + i_y * pixel_height;

            for (i_x = 0; i_x < image_size; i_x++) {

                c_x = c_x_min + i_x * pixel_width;

                z_x = 0.0;
                z_y = 0.0;

                z_x_squared = 0.0;
                z_y_squared = 0.0;

                for(iteration = 0;
                    iteration < iteration_max && \
                    ((z_x_squared + z_y_squared) < escape_radius_squared);
                    iteration++) {

                    z_y = 2 * z_x * z_y + c_y;
                    z_x = z_x_squared - z_y_squared + c_x;

                    z_x_squared = z_x * z_x;
                    z_y_squared = z_y * z_y;
                };

                update_rgb_buffer(iteration, i_x, i_y);

            };

            omp_set_lock(&stacklock);
            i_y = current_i_y;
            current_i_y+=ntasks;
            omp_unset_lock(&stacklock);

        };

    };

    //Buffer das mensagens
    int buf_size = ((int) ceil(image_size/ntasks)) * image_size;
    int* buffer = malloc(buf_size * sizeof(int));
    int counter, i_x, i_y;
    //Mensagens recebidas
    for (int rec = 0; rec < ntasks; rec++) {
        if (rec != taskid) {
            counter = 0;
            MPI_Recv(buffer, buf_size, MPI_INT, rec, rec, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (i_y = rec; i_y < image_size; i_y+=ntasks) {
                for (i_x = 0; i_x < image_size; i_x++) {
                    update_rgb_buffer(buffer[image_size*counter + i_x], i_x, i_y);
                };
                counter++;
            };
        };
    };

};

void mandelbrot(int ntasks, int taskid) {

    //Buffer local
    int buf_size = ((int) ceil(image_size/ntasks)) * image_size;
    int* local_buffer;
    int tid;

    local_buffer = malloc(buf_size * sizeof(int));
    #pragma omp parallel private(nthreads, tid)
    {

        tid = omp_get_thread_num();

        int iteration;

        double c_x;
        double c_y;
        double z_x;
        double z_y;
        double z_x_squared;
        double z_y_squared;
        double escape_radius_squared = 4;

        int i_y;
        int i_x;

        omp_set_lock(&stacklock);
        i_y = current_i_y;
        current_i_y+=ntasks;
        omp_unset_lock(&stacklock);

        while (i_y < image_size) {

            c_y = c_y_min + i_y * pixel_height;

            for (i_x = 0; i_x < image_size; i_x++) {

                c_x = c_x_min + i_x * pixel_width;

                z_x = 0.0;
                z_y = 0.0;

                z_x_squared = 0.0;
                z_y_squared = 0.0;

                for(iteration = 0;
                    iteration < iteration_max && \
                    ((z_x_squared + z_y_squared) < escape_radius_squared);
                    iteration++) {

                    z_y = 2 * z_x * z_y + c_y;
                    z_x = z_x_squared - z_y_squared + c_x;

                    z_x_squared = z_x * z_x;
                    z_y_squared = z_y * z_y;
                };

                local_buffer[(image_size * (i_y-taskid)/ntasks) + i_x] = iteration;

            };

            omp_set_lock(&stacklock);
            i_y = current_i_y;
            current_i_y+=ntasks;
            omp_unset_lock(&stacklock);

        };

    };

    MPI_Send(local_buffer, buf_size, MPI_INT, MASTER, taskid, MPI_COMM_WORLD);

};


int main(int argc, char *argv[]){

    // clock_t start, stop;
    struct timespec start_a, start_b, start_c, start_d, start_e, stop, start, end;
    float duration;

    //MPI Init
    int ntasks;
    int taskid;

    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    //Init
    clock_gettime(CLOCK_MONOTONIC, &start_a);
    init(argc, argv);
    current_i_y = taskid;

    if (taskid == MASTER) {
        clock_gettime(CLOCK_MONOTONIC, &start_b);
        allocate_image_buffer();

        clock_gettime(CLOCK_MONOTONIC, &start_c);
        master_mandelbrot(ntasks, taskid);

        clock_gettime(CLOCK_MONOTONIC, &start_d);
        write_to_file();
    } else {
        clock_gettime(CLOCK_MONOTONIC, &start_e);
        mandelbrot(ntasks, taskid);
    };  

    clock_gettime(CLOCK_MONOTONIC, &stop);

    

    for(int i = 0; i < argc; i++){
        printf("%s ",argv[i]);
    }

    printf(";");

    // printf( "1.Init.........: %.5lf seconds\n", duration );
    duration =  ((double)start_b.tv_sec + 1.0e-9*start_b.tv_nsec) -
                ((double)start_a.tv_sec + 1.0e-9*start_a.tv_nsec); 
    printf( "%.5lf;", duration );

    // printf( "2.Allocate.....: %.5lf seconds\n", duration );
    duration =  ((double)start_c.tv_sec + 1.0e-9*start_c.tv_nsec) - 
                ((double)start_b.tv_sec + 1.0e-9*start_b.tv_nsec); 
    printf( "%.5lf;", duration );

    // printf( "3.Mandelbrot master...: %.5lf seconds\n", duration );
    duration =  ((double)start_d.tv_sec + 1.0e-9*start_d.tv_nsec) - 
                ((double)start_c.tv_sec + 1.0e-9*start_c.tv_nsec); 
    printf( "%.5lf;", duration );


    // printf( "4.Write_file...: %.5lf seconds\n", duration );
    duration =  ((double)start_e.tv_sec + 1.0e-9*start_e.tv_sec) - 
                ((double)start_d.tv_sec + 1.0e-9*start_d.tv_nsec); 
    printf( "%.5lf;", duration );

    // printf( "5.Mandelbrot ...: %.5lf seconds\n", duration );
    duration =  ((double)stop.tv_sec + 1.0e-9*stop.tv_nsec) - 
                ((double)start_e.tv_sec + 1.0e-9*start_e.tv_nsec); 
    printf( "%.5lf;", duration );

    //duration = ( double ) ( stop - start_a ) / CLOCKS_PER_SEC;
    duration =  ((double)stop.tv_sec + 1.0e-9*stop.tv_nsec) - 
                ((double)start_a.tv_sec + 1.0e-9*start_a.tv_nsec); 
    printf( "%.5lf;\n", duration );

    MPI_Finalize();

    return 0;

};
