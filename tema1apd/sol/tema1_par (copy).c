/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

pthread_barrier_t b;
int widthg,heightg;
int **resultat;
params parametrii;

void *f(void *arg)
{
	
	if (thread_id == 1) {
		printf("1\n");
	}
	

	if (thread_id == 0) {
		printf("2\n");
	}

	pthread_exit(NULL);



	////

	int w, h, i;
	int thread_id = *(int *)arg;

	for (i = starthj[thread_id]; i < stophj[thread_id]; i++) {
		resultat[i] = malloc(widthg * sizeof(int));
		if (resultat[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	pthread_barrier_wait(&b);

	for (w = start[thread_id]; w < stop[thread_id]; w++) {
		for (h = 0; h < height; h++) {
			int step = 0;
			complex z = { .a = w * parametrii->resolution + parametrii->x_min,
							.b = h * parametrii->resolution + parametrii->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < parametrii->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + parametrii->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + parametrii->c_julia.b;

				step++;
			}

			resultat[h][w] = step % 256;
		}
	}

	pthread_barrier_wait(&b);



	// transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = starthj2[thread_id]; i <stophj2[thread_id]; i++) {
		int *aux = resultat[i];
		resultat[i] = resultat[height - i - 1];
		resultat[height - i - 1] = aux;
	}

	pthread_barrier_wait(&b);
}

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P;
int *startwj;
int *stopwj;
int *starthj;
int *stophj;
int *starthj2;
int *stophj2;

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
void write_output_file(char *out_filename, int **result, int width, int height)
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
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
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
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

// ruleaza algoritmul Julia
void run_julia(params *par, int **result, int width, int height)
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
}

// ruleaza algoritmul Mandelbrot
void run_mandelbrot(params *par, int **result, int width, int height)
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
}

int main(int argc, char *argv[])
{
	
	int width, height;
	

	// se citesc argumentele programului
	get_args(argc, argv);

	stopwj = malloc((P) * sizeof(int));
	if (stop == NULL) {
		printf("Eroare la malloc stop!\n");
		exit(1);
	}
	startwj = malloc((P) * sizeof(int));
	if (start == NULL) {
		printf("Eroare la malloc start!\n");
		exit(1);
	}

	stophj = malloc((P) * sizeof(int));
	if (stop == NULL) {
		printf("Eroare la malloc stop!\n");
		exit(1);
	}
	starthj = malloc((P) * sizeof(int));
	if (start == NULL) {
		printf("Eroare la malloc start!\n");
		exit(1);
	}

	



	int i, r;
	void *status;
	pthread_t threads[P];
	int arguments[P];
	r=pthread_barrier_init(&b,NULL,P);
	if (r) {
			printf("Eroare la crearea barierei \n");
			exit(-1);
		}

	

	/*for (i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}*/



	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_julia, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;
	widthg=width;
	heightg=height;

	for (i = 0; i < P; i++) {
		startwj[i] = i * (double)width / P;
		stopwj[i] = (i + 1) * (double)width / P;
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	int h2=height/2;

	for (i = 0; i < P; i++) {
		starthj2[i] = i * (double)(h2) / P;
		stophj2[i] = (i + 1) * (double)(h2) / P;
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}
	for (i = 0; i < P; i++) {
		starthj[i] = i * (double)(height) / P;
		stophj[i] = (i + 1) * (double)(height) / P;
		//pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	resultat = allocate_memory(width, height);
	run_julia(&par, resultat, width, height);
	for (i = 0; i < P; i++) {
		arguments[i] = i;
		r = pthread_create(&threads[i], NULL, f, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}
	write_output_file(out_filename_julia, resultat, width, height);
	free_memory(resultat, height);

	// Mandelbrot:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_mandelbrot, &par);

	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;

	resultat = allocate_memory(width, height);
	run_mandelbrot(&par, resultat, width, height);
	write_output_file(out_filename_mandelbrot, resultat, width, height);
	free_memory(resultat, height);

	return 0;
}
