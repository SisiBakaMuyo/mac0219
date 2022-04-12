#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>


double c_x_min;
double c_x_max;
double c_y_min;
double c_y_max;

double pixel_width;
double pixel_height;

int iteration_max = 200;

int image_size;
unsigned char **image_buffer;

int i_x_max;
int i_y_max;
int image_buffer_size;

int nthreads;
int current_iy;
pthread_mutex_t stacklock = PTHREAD_MUTEX_INITIALIZER;

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
                        {16, 16, 16},
                    };

void allocate_image_buffer(){
    clock_t start, stop;
    double duration;

    start = clock();

    int rgb_size = 3;
    image_buffer = (unsigned char **) malloc(sizeof(unsigned char *) * image_buffer_size);

    for(int i = 0; i < image_buffer_size; i++){
        image_buffer[i] = (unsigned char *) malloc(sizeof(unsigned char) * rgb_size);
    };

    stop = clock();
    duration = ( double ) ( stop - start ) / CLOCKS_PER_SEC;

    printf( "2.Allocate.....: %.5lf seconds\n", duration );
};

void init(int argc, char *argv[]){
    clock_t start, stop;
    double duration;

    start = clock();

    if(argc < 6){
        printf("usage: ./mandelbrot_pth c_x_min c_x_max c_y_min c_y_max image_size\n");
        printf("examples with image_size = 11500:\n");
        printf("    Full Picture:         ./mandelbrot_pth -2.5 1.5 -2.0 2.0 11500\n");
        printf("    Seahorse Valley:      ./mandelbrot_pth -0.8 -0.7 0.05 0.15 11500\n");
        printf("    Elephant Valley:      ./mandelbrot_pth 0.175 0.375 -0.1 0.1 11500\n");
        printf("    Triple Spiral Valley: ./mandelbrot_pth -0.188 -0.012 0.554 0.754 11500\n");
        exit(0);
    }
    else{
        sscanf(argv[1], "%lf", &c_x_min);
        sscanf(argv[2], "%lf", &c_x_max);
        sscanf(argv[3], "%lf", &c_y_min);
        sscanf(argv[4], "%lf", &c_y_max);
        sscanf(argv[5], "%d", &nthreads);
        sscanf(argv[6], "%d", &image_size);

        i_x_max           = image_size;
        i_y_max           = image_size;
        image_buffer_size = image_size * image_size;

        pixel_width       = (c_x_max - c_x_min) / i_x_max;
        pixel_height      = (c_y_max - c_y_min) / i_y_max;
    };
    stop = clock();
    duration = ( double ) ( stop - start ) / CLOCKS_PER_SEC;

    printf( "1.Init.........: %.5lf seconds\n", duration );
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

void write_to_file(){
    FILE * file;
    char * filename               = "output.ppm";
    char * comment                = "# ";

    clock_t start, stop;
    double duration;

    start = clock();

    int max_color_component_value = 255;

    file = fopen(filename,"wb");

    fprintf(file, "P6\n %s\n %d\n %d\n %d\n", comment,
            i_x_max, i_y_max, max_color_component_value);

    for(int i = 0; i < image_buffer_size; i++){
        fwrite(image_buffer[i], 1 , 3, file);
    };

    fclose(file);

    stop = clock();
    duration = ( double ) ( stop - start ) / CLOCKS_PER_SEC;

    printf( "4.Write__file..: %.5lf seconds\n", duration );
};


void *compute_mandelbrot(void *threadid){
    double z_x;
    double z_y;
    double z_x_squared;
    double z_y_squared;
    double escape_radius_squared = 4;

    int iteration;
    int i_x;
    int i_y;

    double c_x;
    double c_y;

    pthread_mutex_lock(&stacklock);
    i_y = current_iy++;
    pthread_mutex_unlock(&stacklock);

    while (i_y < image_size){
        c_y = c_y_min + i_y * pixel_height;

        if(fabs(c_y) < pixel_height / 2){
            c_y = 0.0;
        };

        for(i_x = 0; i_x < i_x_max; i_x++){
            c_x         = c_x_min + i_x * pixel_width;

            z_x         = 0.0;
            z_y         = 0.0;

            z_x_squared = 0.0;
            z_y_squared = 0.0;

            for(iteration = 0;
                iteration < iteration_max && \
                ((z_x_squared + z_y_squared) < escape_radius_squared);
                iteration++){
                z_y         = 2 * z_x * z_y + c_y;
                z_x         = z_x_squared - z_y_squared + c_x;

                z_x_squared = z_x * z_x;
                z_y_squared = z_y * z_y;
            };

            update_rgb_buffer(iteration, i_x, i_y);
        };

        pthread_mutex_lock(&stacklock);
        i_y = current_iy++;
        pthread_mutex_unlock(&stacklock);
    };

    pthread_exit(NULL);
    
};

int main(int argc, char *argv[]){
    clock_t start, stop;
    double duration;

    init(argc, argv);

    allocate_image_buffer();

    start = clock();

    //pthread
    pthread_t *threads = malloc(sizeof(pthread_t)*nthreads);
    int errorcode;
    long t;
    for(t = 0; t < nthreads; t++) {
        errorcode = pthread_create(&threads[t], NULL, compute_mandelbrot, (void *) t);
        if (errorcode) {
            printf("ERROR pthread_create(): %d\n", errorcode);
            exit(-1);
        };
    };
    for(t = 0; t < nthreads; t++) {
        pthread_join(threads[t], NULL);
    }
    //fim da pthread

    stop = clock();
    duration = ( double ) ( stop - start ) / CLOCKS_PER_SEC;

    printf( "3.Mandelbrot...: %.5lf seconds\n", duration/nthreads );

    write_to_file();

    return 0;
};
