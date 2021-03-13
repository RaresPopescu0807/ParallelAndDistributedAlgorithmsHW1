/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P;
params paramJ, paramM;
int *startwj;
int *stopwj;
int *starthj;
int *stophj;
int *starthj2;
int *stophj2;
int *startwm;
int *stopwm;
int *starthm;
int *stophm;
int *starthm2;
int *stophm2;
int widthJ,heightJ,widthM,heightM;
int **result;
pthread_barrier_t b;


// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 5) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
/*void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}*/

// aloca memorie pentru rezultat
/*int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}*/

// elibereaza memoria alocata
/*void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}*/

// ruleaza algoritmul Julia
/*void run_julia(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par->c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
}*/

// ruleaza algoritmul Mandelbrot
/*void run_mandelbrot(params *par, int **result, int width, int height)
{
	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = 0; h < height; h++) {
			complex c = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	// transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = 0; i < height / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
}*/



void *thread_function(void *arg)
{
	int thread_id = *(int *)arg;
	int i,j,w,h;

	//aloc julia
	if(thread_id==0)
	{
		result = malloc(heightJ * sizeof(int*));
		
		if (result == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	//printf("start e %d\n",starthj[thread_id] );
	//printf("stop e %d\n",stophj[thread_id] );
	//printf("size %d\n",heightJ );
	//printf("Th id e %d\n",result );

	pthread_barrier_wait(&b);

	for (i = starthj[thread_id]; i < stophj[thread_id]; i++) {

		result[i] = malloc(widthJ * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	pthread_barrier_wait(&b);

	//julia


	for (w = startwj[thread_id]; w < stopwj[thread_id]; w++) {
		for (h = 0; h < heightJ; h++) {
			int step = 0;
			complex z = { .a = w * paramJ.resolution + paramJ.x_min,
							.b = h * paramJ.resolution + paramJ.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < paramJ.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + paramJ.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + paramJ.c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	pthread_barrier_wait(&b);

	//schimbare coordonate

	for (i = starthj2[thread_id]; i <  stophj2[thread_id]; i++) {
		int *aux = result[i];
		result[i] = result[heightJ - i - 1];
		result[heightJ - i - 1] = aux;
	}

	pthread_barrier_wait(&b);

	//scriere fisier


	if(thread_id==0){
	FILE *file = fopen(out_filename_julia, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		//return;
	}

	fprintf(file, "P2\n%d %d\n255\n", widthJ, heightJ);
	for (i = 0; i < heightJ; i++) {
		for (j = 0; j < widthJ; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	}

	pthread_barrier_wait(&b);

	for (i = starthj[thread_id]; i < stophj[thread_id]; i++) {
		free(result[i]);
	}
	pthread_barrier_wait(&b);

	if(thread_id==0)
		free(result);






	if(thread_id==0)
	{
		result = malloc(heightM * sizeof(int*));
		
		if (result == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	//printf("start e %d\n",starthj[thread_id] );
	//printf("stop e %d\n",stophj[thread_id] );
	//printf("size %d\n",heightJ );
	//printf("Th id e %d\n",result );

	pthread_barrier_wait(&b);

	for (i = starthm[thread_id]; i < stophm[thread_id]; i++) {

		result[i] = malloc(widthM * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	pthread_barrier_wait(&b);

	//julia
	//printf("cva\n");
	//printf("%d %d\n",startwm[thread_id],stopwm[thread_id] );
	//printf("%d %d\n",starthm[thread_id],stophm[thread_id] );


	for (w = startwm[thread_id]; w < stopwm[thread_id]; w++) {

		for (h = 0; h < heightM; h++) {
			complex c = { .a = w * paramM.resolution + paramM.x_min,
							.b = h * paramM.resolution + paramM.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < paramM.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}
			//printf("%d\n",h);

			result[h][w] = step % 256;
		}
		//printf("cva\n");
	}
	//printf("cva\n");

	pthread_barrier_wait(&b);

	//schimbare coordonate

	for (i = starthm2[thread_id]; i <  stophm2[thread_id]; i++) {
		int *aux = result[i];
		result[i] = result[heightM - i - 1];
		result[heightM - i - 1] = aux;
	}

	pthread_barrier_wait(&b);

	//scriere fisier


	if(thread_id==0){
	FILE *file = fopen(out_filename_mandelbrot, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		//return;
	}

	fprintf(file, "P2\n%d %d\n255\n", widthM, heightM);
	for (i = 0; i < heightM; i++) {
		for (j = 0; j < widthM; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	}

	pthread_barrier_wait(&b);

	for (i = starthm[thread_id]; i < stophm[thread_id]; i++) {
		free(result[i]);
	}
	pthread_barrier_wait(&b);

	if(thread_id==0)
		free(result);

	






	



	//pthread_barrier_wait(&b);
	pthread_exit(NULL);

	






}



int main(int argc, char *argv[])
{
	int r,i;
	pthread_t tid[P];
	int thread_id[P];
	
	
	//run ./tests/julia1.in j ./tests/mandelbrot1.in m 2

	// se citesc argumentele programului
	get_args(argc, argv);

	

	read_input_file(in_filename_julia, &paramJ);
	read_input_file(in_filename_mandelbrot, &paramM);

	startwj= malloc(P * sizeof(int));
	stopwj= malloc(P * sizeof(int));
	starthj= malloc(P * sizeof(int));
	stophj= malloc(P * sizeof(int));
	starthj2= malloc(P * sizeof(int));
	stophj2= malloc(P * sizeof(int));
	startwm= malloc(P * sizeof(int));
	stopwm= malloc(P * sizeof(int));
	starthm= malloc(P * sizeof(int));
	stophm= malloc(P * sizeof(int));
	starthm2= malloc(P * sizeof(int));
	stophm2= malloc(P * sizeof(int));

	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	

	widthJ = (paramJ.x_max - paramJ.x_min) / paramJ.resolution;
	heightJ = (paramJ.y_max - paramJ.y_min) / paramJ.resolution;

	widthM = (paramM.x_max - paramM.x_min) / paramM.resolution;
	heightM = (paramM.y_max - paramM.y_min) / paramM.resolution;

	for (i = 0; i < P; i++) {
		startwj[i] = i * (double)widthJ / P;
		stopwj[i] = MIN((i + 1) * (double)widthJ / P,widthJ);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	int hj2=heightJ/2;

	for (i = 0; i < P; i++) {
		starthj2[i] = i * (double)(hj2) / P;
		stophj2[i] = MIN((i + 1) * (double)(hj2) / P,hj2);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	for (i = 0; i < P; i++) {
		starthj[i] = i * (double)(heightJ) / P;
		stophj[i] = MIN((i + 1) * (double)(heightJ) / P,heightJ);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}



	for (i = 0; i < P; i++) {
		startwm[i] = i * (double)widthM / P;
		stopwm[i] = MIN((i + 1) * (double)widthM / P,widthM);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	int hm2=heightM/2;

	for (i = 0; i < P; i++) {
		starthm2[i] = i * (double)(hm2) / P;
		stophm2[i] = MIN((i + 1) * (double)(hm2) / P,hm2);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	for (i = 0; i < P; i++) {
		starthm[i] = i * (double)(heightM) / P;
		stophm[i] = MIN((i + 1) * (double)(heightM) / P,heightM);
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	r=pthread_barrier_init(&b,NULL,P);

	if (r) {
			//printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}

	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	// se asteapta thread-urile
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}





	//result = allocate_memory(width, height);


	//run_julia(&par, result, width, height);
	//write_output_file(out_filename_julia, result, width, height);
	//free_memory(result, height);

	// Mandelbrot:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata

	/*
	read_input_file(in_filename_mandelbrot, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;

	result = allocate_memory(width, height);
	run_mandelbrot(&par, result, width, height);
	write_output_file(out_filename_mandelbrot, result, width, height);
	free_memory(result, height);*/




	return 0;
}

