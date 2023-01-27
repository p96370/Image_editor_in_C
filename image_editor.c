#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "apply_values.h"

// frees a matrix
void free_matrix(image **mat, int line, int col)
{
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < col; j++)
			free(mat[i][j].pixel);
		free(mat[i]);
	}
	free(mat);
}

// allocs a matrix of type image
image **alloc_matrix_image(int line, int col, int dim)
{
	image **mat = (image **)calloc(line, sizeof(image *));
	if (!mat) {
		fprintf(stderr, "Malloc failed\n");
		return NULL;
	}
	for (int i = 0; i < line; i++) {
		mat[i] = (image *)calloc(col, sizeof(image));
		for (int j = 0; j < col; j++)
			mat[i][j].pixel = (unsigned char *)calloc(dim, sizeof(unsigned char));
	}
	return mat;
}

// reads the matrix of a picture from a file, either binary or as a text
image **read_matrix(FILE *f, int line, int col, int type, int size)
{
	image **mat = NULL;
	mat = (image **)malloc(line * sizeof(image *));

	if (!mat) {
		fprintf(stderr, "Malloc failed\n");
		return NULL;
	}

	for (int i = 0; i < line; i++) {
		mat[i] = (image *)malloc(col * sizeof(image));
		for (int j = 0; j < col; j++)
			mat[i][j].pixel = (unsigned char *)malloc(size * sizeof(unsigned char));
	}

	for (int i = 0; i < line; i++) {
		for (int j = 0; j < col; j++) {
			if (type == ASCII) {
				for (int k = 0; k < size; k++)
					fscanf(f, "%hhu", &mat[i][j].pixel[k]);
			} else {
				for (int k = 0; k < size; k++)
					fread(&mat[i][j].pixel[k], sizeof(unsigned char), 1, f);
			}
		}
	}
	return mat;
}

// acces the matrix of the given image if possible, if not prints message
image **load_matrix(char *filename, int *line, int *col, int *max_value,
					int *size, int *type)
{
	FILE *f;
	f = fopen(filename, "rt");
	if (!f) {
		printf("Failed to load %s\n", filename);
		return NULL;
	}
	char p;
	int nr, maxi = INVALID, n = INVALID, m = INVALID;
	// Reads the type of file, dimensions and maximum value of the matrix
	fscanf(f, "%c%d", &p, &nr);
	fscanf(f, "%d%d", &m, &n);
	if (nr == 2 || nr == 3 || nr == 5 || nr == 6)
		fscanf(f, "%d", &maxi);
	*line = n;
	*col = m;
	*max_value = maxi;

	image **mat = NULL;
	// returns the current position of the cursor in the file
	int current_poz = ftell(f);
	if (nr == 2 || nr == 3) {
		if (nr == 2)
			*size = 1;
		else
			*size = 3;
		// ascii image to read
		*type = ASCII;
		mat = read_matrix(f, *line, *col, ASCII, *size);
		printf("Loaded %s\n", filename);
		fclose(f);
	}
	if (nr == 5 || nr == 6) {
		fclose(f);
		FILE *f = fopen(filename, "rb");
		if (!f) {
			printf("Failed to load %s\n", filename);
			return NULL;
		}
		if (nr == 5)
			*size = 1;
		else
			*size = 3;
		// binary image to read
		*type = BINARY;
		printf("Loaded %s\n", filename);
		fseek(f, current_poz + 1, SEEK_SET);
		mat = read_matrix(f, *line, *col, BINARY, *size);
		fclose(f);
	}
	return mat;
}

// check if the given coordinates are valid
int check_select(image **mat, int line, int col, int x1, int x2, int y1,
				 int y2)
{
	if (!mat) {
		printf("No image loaded\n");
		return 0;
	}
	if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	if (x2 > col || x1 >= col || y1 >= line || y2 > line) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	return 1;
}

// select the coordinates of the matrix
void select_mat(int *x1, int *y1, int *x2, int *y2, int a, int b, int c,
				int d)
{
	*x1 = a;
	*y1 = b;
	*x2 = c;
	*y2 = d;
	if (*x1 > *x2) {
		int aux;
		aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux;
		aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}
	printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
}

// crops the current image using the current coordinates
image **crop(image **mat, int *line, int *col, int size, int x1, int y1,
			 int x2, int y2)
{
	if (!mat) {
		printf("No image loaded\n");
		return NULL;
	}
	if (y1 == 0 && x1 == 0 && y2 == *line && x2 == *col) {
		printf("Image cropped\n");
		return mat;
	}
	// allocs a  new matrix with the new dimensions
	image **mat_cut = alloc_matrix_image(y2 - y1, x2 - x1, size);

	// puts the right values in the new matrix
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int k = 0; k < size; k++)
				mat_cut[i - y1][j - x1].pixel[k] = mat[i][j].pixel[k];

	free_matrix(mat, *line, *col);
	// the dimensions are modified
	*line = y2 - y1;
	*col = x2 - x1;
	printf("Image cropped\n");

	return mat_cut;
}

// sets the current coordinates to full image
void init_crop(int *x1, int *y1, int *x2, int *y2, int line, int col)
{
	*x1 = 0;
	*y1 = 0;
	*x2 = col;
	*y2 = line;
}

// converts the string s to an integer number
int convert_char_to_int(char *s)
{
	int p = 1, nr = 0, l = strlen(s);
	for (int i = l - 1; i > 0; i--) {
		nr = (s[i] - '0') * p + nr;
		p *= 10;
	}
	if (s[0] != '-')
		nr = (s[0] - '0') * p + nr;
	else
		nr *= (-1);

	return nr;
}

// calculates and prints the histogram of the image
void histog(int max_value, image **mat, int line, int col, int size)
{
	int nr_given_stars = -1, nr_given_bins = -1;
	char s[LENGTH];
	fgets(s, LENGTH, stdin);
	s[strlen(s) - 1] = '\0';
	int nr = 0;
	char *p = strtok(s, " ");
	while (p) {
		nr++;
		if (nr == 1)
			nr_given_stars = convert_char_to_int(p);
		if (nr == 2)
			nr_given_bins = convert_char_to_int(p);
		p = strtok(NULL, " ");
	}
	if (!mat) {
		printf("No image loaded\n");
		return;
	}
	if (nr != 2) {
		printf("Invalid command\n");
		return;
	}
	if (size == 3) {
		printf("Black and white image needed\n");
		return;
	}
	int *freq_elem = NULL;
	// I calculate the frequence of each element in the matrix
	freq_elem = (int *)calloc(max_value, sizeof(int));
	for (int i = 0; i < line; i++)
		for (int j = 0; j < col; j++)
			for (int k = 0; k < size; k++)
				freq_elem[mat[i][j].pixel[k]]++;

	// the number of intervals, given by the mathematical formula
	int nr_bins = 256 / nr_given_bins, len = 0;
	int *freq_bin = (int *)calloc(256, sizeof(int));
	for (int i = 0; i < 256; i += nr_bins) {
		int freq_max_elem = 0;
		for (int j = i; j < i + nr_bins; j++)
			freq_max_elem += freq_elem[j];
		freq_bin[len++] = freq_max_elem;
	}
	// what is the highest value of all bins
	int maxi = 0;
	for (int i = 0; i < len; i++)
		if (freq_bin[i] > maxi)
			maxi = freq_bin[i];

	// print the corect number of stars on each line
	for (int i = 0; i < len; i++) {
		int nr_stars = (freq_bin[i] * nr_given_stars) / maxi;
		printf("%d\t|\t", nr_stars);
		for (int j = 0; j < nr_stars; j++)
			printf("*");
		printf("\n");
	}
	// free the arrays
	free(freq_elem);
	free(freq_bin);
}

// check if the current picture can be rotated
int check_rotate(image **mat, int x1, int x2, int y1, int y2, int angle)
{
	if (!mat) {
		printf("No image loaded\n");
		return 0;
	}
	if (x2 - x1 != y2 - y1) {
		printf("The selection must be square\n");
		return 0;
	}
	if (angle != 90 && angle != 180 && angle != 270 && angle != 360) {
		printf("Unsupported rotation angle\n");
		return 0;
	}
	return 1;
}

// it rotates the image to right by 90 degrees
void rotate_right_90(image **mat, image **new_mat, int x1, int x2, int y1,
					 int y2)
{
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			new_mat[y1 + j - x1][x2 - 1 - i + y1] = mat[i][j];
}

// it rotates the image to right by 180 degrees
void rotate_right_180(image **mat, image **new_mat, int x1, int x2, int y1,
					  int y2)
{
	rotate_right_90(mat, new_mat, x1, x2, y1, y2);
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			mat[i][j] = new_mat[i][j];

	rotate_right_90(mat, new_mat, x1, x2, y1, y2);
}

// it rotates the image to right by 270 degrees
void rotate_right_270(image **mat, image **new_mat, int x1, int x2, int y1,
					  int y2)
{
	rotate_right_180(mat, new_mat, x1, x2, y1, y2);
	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			mat[i][j] = new_mat[i][j];

	rotate_right_90(mat, new_mat, x1, x2, y1, y2);
}

// makes swap of 2 integers
void swap_int(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

// rotates a rectangulare picture because it is at full size
void rotate_rectangle(image ***m, int *line, int *col, int *x2, int *y2,
					  int angle, int size)
{
	int nr = 90, l = *line, c = *col;
	// allocs a new matrix that is a copy of the initial one
	image **mat = alloc_matrix_image(*line, *col, size);
	for (int i = 0; i < *line; i++)
		for (int j = 0; j < *col; j++)
			for (int k = 0; k < size; k++)
				mat[i][j].pixel[k] = (*m)[i][j].pixel[k];

	// rotates 90 degrees right once, twice or thrice
	while (angle) {
		// allocs a new matrix with swapped dimensions
		image **new_mat = NULL;
		new_mat = (image **)malloc((*col) * sizeof(image *));
		for (int i = 0; i < *col; i++) {
			new_mat[i] = (image *)malloc((*line) * sizeof(image));
			for (int j = 0; j < *line; j++)
				new_mat[i][j].pixel = (unsigned char *)malloc(size * sizeof(unsigned char));
		}

		// puts the right values in the new matrix
		for (int i = 0; i < *line; i++)
			for (int j = 0; j < *col; j++)
				for (int k = 0; k < size; k++)
					new_mat[j][*line - 1 - i].pixel[k] = mat[i][j].pixel[k];

		if (*col < *line) {
			mat = (image **)realloc(mat, (*col) * sizeof(image *));
			for (int  i = 0; i < *col; i++) {
				for (int j = 0; j < *col; j++)
					free(mat[i][j].pixel);
				free(mat[i]);
				mat[i] = NULL;
			}
			for (int i = 0; i < *col; i++) {
				mat[i] = (image *)malloc((*line) * sizeof(image));
				for (int j = 0; j < *line; j++)
					mat[i][j].pixel = (unsigned char *)malloc(size * sizeof(unsigned char));
			}

		} else {
			for (int i = 0; i < *line; i++)
				mat[i] = (image *)realloc(mat[i], (*line) * sizeof(image));
			mat = (image **)realloc(mat, (*col) * sizeof(image *));
			for (int i = *line; i < *col; i++) {
				mat[i] = (image *)malloc((*line) * sizeof(image));
				for (int j = 0; j < *line; j++)
					mat[i][j].pixel = (unsigned char *)malloc(size * sizeof(unsigned char));
			}
		}
		// copies the new values in the initial matrix
		for (int i = 0; i < *col; i++)
			for (int j = 0; j < *line; j++)
				for (int k = 0; k < size; k++)
					mat[i][j].pixel[k] = new_mat[i][j].pixel[k];

		angle -= 90;
		nr += 90;
		free_matrix(new_mat, *col, *line);
		// dimensions are swapped because the picture turned
		swap_int(col, line);
		swap_int(x2, y2);
	}
	free_matrix(*m, l, c);
	// copies all the values back to the actual initial picture
	*m = alloc_matrix_image(*line, *col, size);
	for (int i = 0; i < *line; i++)
		for (int j = 0; j < *col; j++)
			for (int k = 0; k < size; k++)
				(*m)[i][j].pixel[k] = mat[i][j].pixel[k];

	free_matrix(mat, *line, *col);
}

// makes the right rotations and prints the necessary messages
void rotate_matrix(image ***mat, int x1, int *x2, int y1, int *y2, int size,
				   int *line, int *col)
{
	int angle;
	char sign;
	char s[LENGTH];
	fgets(s, LENGTH, stdin);
	s[strlen(s) - 1] = '\0';
	// when sign is = it means that in the input file there is no sign
	// and it means to rotate to right by default
	if (strchr(s, '-')) {
		sign = '-';
		angle = convert_char_to_int(s + 2);
	} else {
		if (strchr(s, '+')) {
			angle = convert_char_to_int(s + 2);
			sign = '+';
		} else {
			angle = convert_char_to_int(s + 1);
			sign = '=';
		}
	}
	if (!(*mat)) {
		printf("No image loaded\n");
		return;
	}
	// separate case in angle in 0
	if (angle == 0) {
		if (sign != '=')
			printf("Rotated %c%d\n", sign, angle);
		else
			printf("Rotated %d\n", angle);
		return;
	}
	if (angle % 10 != 0 || angle > 360 || angle < -360) {
		printf("Unsupported rotation angle\n");
		return;
	}
	if (!x1 && !y1 && ((!(*x2) && !(*y2)) || (*x2 == *col && *y2 == *line))) {
		int cop_angle = angle;
		if (sign == '-' && angle != 360)
			angle = 360 - angle;
		rotate_rectangle(mat, line, col, x2, y2, angle, size);
		if (sign != '=')
			printf("Rotated %c%d\n", sign, cop_angle);
		else
			printf("Rotated %d\n", cop_angle);
		return;
	}
	// if the picture is a square
	if (check_rotate(*mat, x1, *x2, y1, *y2, angle)) {
		image **new_mat = alloc_matrix_image(*line, *col, size);
		if (sign == '+' || sign == '=') {
			if (angle == 90)
				rotate_right_90(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle == 180)
				rotate_right_180(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle == 270)
				rotate_right_270(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle != 360)
				for (int i = y1; i < *y2; i++)
					for (int j = x1; j < *x2; j++)
						(*mat)[i][j] = new_mat[i][j];
		} else {
			if (angle == 90)
				rotate_right_270(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle == 180)
				rotate_right_180(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle == 270)
				rotate_right_90(*mat, new_mat, x1, *x2, y1, *y2);
			if (angle != 360)
				for (int i = y1; i < *y2; i++)
					for (int j = x1; j < *x2; j++)
						(*mat)[i][j] = new_mat[i][j];
		}
		if (sign != '=')
			printf("Rotated %c%d\n", sign, angle);
		else
			printf("Rotated %d\n", angle);
	}
}

// exits the program and frees all the memory
int exit_and_free(image **mat, int line, int col)
{
	if (!mat) {
		printf("No image loaded\n");
		return 0;
	}
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < col; j++)
			free(mat[i][j].pixel);
		free(mat[i]);
	}
	free(mat);
	return 1;
}

// prints the picture in the specified file
void print_matrix(FILE *g, image **mat, int line, int col, int size, int type,
				  int maxi)
{
	int nr = -1;
	if (type == ASCII) {
		if (size == 1)
			nr = 2;
		else
			nr = 3;
	} else {
		if (size == 1)
			nr = 5;
		else
			nr = 6;
	}
	// the type, dimensions and max value are written as text
	fprintf(g, "P%d\n", nr);
	fprintf(g, "%d %d\n", col, line);
	fprintf(g, "%d\n", maxi);
	// the matrix is printed as either ascii or binary, depending on the type
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < col; j++) {
			for (int k = 0; k < size; k++)
				if (type == ASCII)
					fprintf(g, "%d ", mat[i][j].pixel[k]);
				else
					fwrite(&mat[i][j].pixel[k], sizeof(unsigned char), 1, g);
		}
		if (type == ASCII)
			fprintf(g, "\n");
	}
}

// saves an image to a specified file if possible,
// otherwise prints error message
void save_image(image **mat, int line, int col, int size, int maxi)
{
	char filename[100];
	fgets(filename, 100, stdin);
	if (!mat) {
		printf("No image loaded\n");
		return;
	}
	// delets the new line character in the name of the file and
	// all the possible spaces
	int ascii = 0;
	if (strstr(filename, "ascii")) {
		ascii = 1;
		filename[strlen(filename) - 6] = '\0';
		int poz = strlen(filename) - 1;
		while (filename[poz] == ' ' && poz >= 0)
			poz--;
		filename[poz + 1] = '\0';
	} else {
		filename[strlen(filename) - 1] = '\0';
	}
	int poz = strlen(filename) - 1, cop;
	cop = poz;
	while (filename[poz] == ' ' && poz >= 0)
		poz--;
	if (poz < cop)
		filename[poz + 1] = '\0';
	while (filename[0] == ' ') {
		char aux[100] = "";
		 strcpy(aux, filename + 1);
		strcpy(filename, aux);
	}
	// opens an output file to print the picture
	// either binary or ascii
	FILE *g = fopen(filename, "wt");
	if (!g) {
		fprintf(stderr, "Cannot open %s\n", filename);
		return;
	}
	if (ascii) {
		print_matrix(g, mat, line, col, size, ASCII, maxi);
		printf("Saved %s\n", filename);
		fclose(g);
	} else {
		fclose(g);
		FILE *g = fopen(filename, "wb");
		if (!g) {
			fprintf(stderr, "Cannot open %s\n", filename);
			return;
		}
		printf("Saved %s\n", filename);
		print_matrix(g, mat, line, col, size, BINARY, maxi);
		fclose(g);
	}
}

// keeps the values of the matrix in range[0, 255]
double clamp(double value)
{
	if (value < 0)
		value = 0;
	if (value > 255)
		value = 255;
	return value;
}

// equalizes the picture as explined and according to the given formula
void equalize(image **mat, int line, int col, int size, int max_value)
{
	if (!mat) {
		printf("No image loaded\n");
		return;
	}
	if (size == 3) {
		printf("Black and white image needed\n");
		return;
	}
	int *frecv = NULL;
	// the frequence of each pixel of the image
	frecv = (int *)calloc((max_value + 1), sizeof(int));
	for (int i = 0; i < line; i++)
		for (int j = 0; j < col; j++)
			for (int k = 0; k < size; k++)
				frecv[mat[i][j].pixel[k]]++;

	double new_value, area;
	area = line * col;
	for (int i = 0; i < line; i++) {
		for (int j = 0; j < col; j++) {
			double sum = 0;
			for (int k = 0; k <= mat[i][j].pixel[0]; k++)
				sum += frecv[k];
			new_value = (255 * sum) / area;
			new_value = clamp(new_value);
			mat[i][j].pixel[0] = round(new_value);
		}
	}
	printf("Equalize done\n");
	free(frecv);
}

// allocs a simple double matrix
double **alloc_matrix(int n, int m)
{
	double **mat = NULL;
	mat = (double **)malloc(n * sizeof(double *));
	for (int i = 0; i < n; i++)
		mat[i] = (double *)malloc(m * sizeof(double));
	return mat;
}

// frees a double matrix
void free_matrix_double(double **mat, int n)
{
	for (int i = 0; i < n; i++)
		free(mat[i]);
	free(mat);
}

// applies the specified filter
void apply_matrix(image ***mat, double filter[3][3], int y1, int y2, int x1,
				  int x2, int line, int col)
{
	double **pixel0 = alloc_matrix(y2 - y1, x2 - x1);
	double **pixel1 = alloc_matrix(y2 - y1, x2 - x1);
	double **pixel2 = alloc_matrix(y2 - y1, x2 - x1);
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			double line1 = 0, line2 = 0, line3 = 0;
			if (i && j && i != line - 1 && j != col - 1) {
				for (int l = -1; l < 2; l++) {
					for (int c = -1; c < 2; c++) {
						double x, y, z;
						x = (*mat)[i + l][j + c].pixel[0] * filter[l + 1][c + 1];
						line1 += (double)x;
						y = (*mat)[i + l][j + c].pixel[1] * filter[l + 1][c + 1];
						line2 += (double)y;
						z = (*mat)[i + l][j + c].pixel[2] * filter[l + 1][c + 1];
						line3 += (double)z;
					}
				}
			}
			pixel0[i - y1][j - x1] = clamp(round(line1));
			pixel1[i - y1][j - x1] = clamp(round(line2));
			pixel2[i - y1][j - x1] = clamp(round(line3));
		}
	}
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			if (i && j && i != line - 1 && j != col - 1) {
				(*mat)[i][j].pixel[0] = (unsigned char)pixel0[i - y1][j - x1];
				(*mat)[i][j].pixel[1] = (unsigned char)pixel1[i - y1][j - x1];
				(*mat)[i][j].pixel[2] = (unsigned char)pixel2[i - y1][j - x1];
			}
		}
	}
	free_matrix_double(pixel0, y2 - y1);
	free_matrix_double(pixel1, y2 - y1);
	free_matrix_double(pixel2, y2 - y1);
}

// checks is APPLY can be done, prints expected messages
// if possible checks which filter to apply
void apply(image **mat, int line, int col, int size, int y1, int y2, int x1,
		   int x2)
{
	char param[100], ch;
	scanf("%c", &ch);
	if (ch == ' ')
		scanf("%s", param);
	if (!mat) {
		printf("No image loaded\n");
		return;
	}
	if (ch == '\n') {
		printf("Invalid command\n");
		return;
	}
	if (size == 1) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int  ok = 0;
	if (strcmp(param, "EDGE") == 0 || strcmp(param, "SHARPEN") == 0)
		ok = 1;
	if (strcmp(param, "BLUR") == 0 || strcmp(param, "GAUSSIAN_BLUR") == 0)
		ok = 1;
	if (!ok) {
		printf("APPLY parameter invalid\n");
		return;
	}
	if (strcmp(param, "EDGE") == 0)
		apply_matrix(&mat, edge, y1, y2, x1, x2, line, col);
	if (strcmp(param, "SHARPEN") == 0)
		apply_matrix(&mat, sharpen, y1, y2, x1, x2, line, col);
	if (strcmp(param, "BLUR") == 0)
		apply_matrix(&mat, blur, y1, y2, x1, x2, line, col);
	if (strcmp(param, "GAUSSIAN_BLUR") == 0)
		apply_matrix(&mat, blur_g, y1, y2, x1, x2, line, col);

	printf("APPLY %s done\n", param);
}

// checks if a string is a number or a sign
int digit(char *s)
{
	int len = strlen(s);
	for (int i = 0; i < len; i++)
		if (!((s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '+'))
			return 0;
	return 1;
}

// checks if a string is a letter
int is_alpha(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return 1;
	return 0;
}

// checks if a character is a sign
int is_sign(char c)
{
	if (c == '+' || c == '-')
		return 1;
	return 0;
}

// checks if an integer is a cipher
int is_digit_int(int x)
{
	if (x >= 0 && x <= 9)
		return 1;
	return 0;
}

// reads parameters for operation sellect
void read_parameters(char *s, char *s1, char *s2, char *s3, char *s4)
{
	int nr = 0;
	char *p = strtok(s, " ");
	while (p) {
		nr++;
		switch (nr) {
		case 1:
			strcpy(s1, p);
			break;
		case 2:
			strcpy(s2, p);
			break;
		case 3:
			strcpy(s3, p);
			break;
		case 4:
			strcpy(s4, p);
			break;
		default:
			break;
		}
		p = strtok(NULL, " ");
	}
}

void select_all(image **mat, int *x1, int *y1, int *x2, int *y2, int line,
				int col)
{
	*x1 = 0;
	*y1 = 0;
	*x2 = col;
	*y2 = line;
	if (!mat)
		printf("No image loaded\n");
	else
		printf("Selected ALL\n");
}

void copy_coord(char *s1, char *s2, char *s3, char *s4, int *cpx1, int *cpy1,
				int *cpx2, int *cpy2)
{
	*cpx1 = convert_char_to_int(s1);
	*cpy1 = convert_char_to_int(s2);
	*cpx2 = convert_char_to_int(s3);
	*cpy2 = convert_char_to_int(s4);
}

int main(void)
{
	int line = 0, col = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	int max_value = INVALID, size, type;
	char command[LENGTH] = "";
	image **mat = NULL;
	do {
		scanf("%s", command);
		int ok = 0;
		if (strcmp(command, "LOAD") == 0) {
			ok = 1;
			if (mat) {
				free_matrix(mat, line, col);
				mat = NULL;
				line = 0;
				col = 0;
			}
			char filename[LENGTH];
			scanf("%s", filename);
			mat = load_matrix(filename, &line, &col, &max_value, &size, &type);
			y2 = line;
			x2 = col;
		}
		if (strcmp(command, "SELECT") == 0) {
			char s[LENGTH];
			fgets(s, LENGTH, stdin);
			if (strstr(s, "ALL")) {
				select_all(mat, &x1, &y1, &x2, &y2, line, col);
			} else {
				char s1[LENGTH], s2[LENGTH], s3[LENGTH], s4[LENGTH];
				s[strlen(s) - 1] = '\0';
				read_parameters(s, s1, s2, s3, s4);
				if (digit(s1) && digit(s2) && digit(s3) && digit(s4)) {
					int cpx1 = 0, cpy1 = 0, cpx2 = 0, cpy2 = 0;
					copy_coord(s1, s2, s3, s4, &cpx1, &cpy1, &cpx2, &cpy2);
					if (check_select(mat, line, col, cpx1, cpx2, cpy1, cpy2))
						select_mat(&x1, &y1, &x2, &y2, cpx1, cpy1, cpx2, cpy2);
				} else {
					printf("Invalid command\n");
				}
			}
			ok = 1;
		}
		if (strcmp(command, "HISTOGRAM") == 0) {
			ok = 1;
			histog(max_value, mat, line, col, size);
		}
		if (strcmp(command, "EQUALIZE") == 0) {
			ok = 1;
			equalize(mat, line, col, size,  max_value);
		}
		if (strcmp(command, "ROTATE") == 0) {
			ok = 1;
			rotate_matrix(&mat, x1, &x2, y1, &y2, size, &line, &col);
		}
		if (strcmp(command, "CROP") == 0) {
			ok = 1;
			mat = crop(mat, &line, &col, size, x1, y1, x2, y2);
			init_crop(&x1, &y1, &x2, &y2, line, col);
		}
		if (strcmp(command, "APPLY") == 0) {
			ok = 1;
			apply(mat, line, col, size, y1, y2, x1, x2);
		}
		if (strcmp(command, "SAVE") == 0) {
			ok = 1;
			save_image(mat, line, col, size, max_value);
		}
		if (strcmp(command, "EXIT") == 0) {
			ok = 1;
			int x = exit_and_free(mat, line, col);
			if (x == 1)
				exit(0);
		}
		if (!ok) {
			fgets(command, LENGTH, stdin);
			printf("Invalid command\n");
		}

	} while (strcmp(command, "EXIT") != 0);
	return 0;
}
