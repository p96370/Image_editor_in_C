#ifndef __TEMA3__
#define __TEMA3__

typedef struct {
	//int *pixel;
	unsigned char *pixel;
} image;

#define LENGTH 100
#define BINARY 0
#define ASCII 1
#define INVALID -1000

void free_matrix(image **mat, int line, int col);
image **alloc_matrix_image(int line, int col, int dim);
image **read_matrix(FILE *f, int line, int col, int type, int size);
image **load_matrix(char *filename, int *line, int *col, int *max_value,
					int *size, int *type);
int check_select(image **mat, int line, int col, int x1, int x2, int y1,
				 int y2);
void select_mat(int *x1, int *y1, int *x2, int *y2, int a, int b, int c,
				int d);
image **crop(image **mat, int *line, int *col, int size, int x1, int y1,
			 int x2, int y2);
void init_crop(int *x1, int *y1, int *x2, int *y2, int line, int col);
int convert_char_to_int(char *s);
void histog(int max_value, image **mat, int line, int col, int size);
int check_rotate(image **mat, int x1, int x2, int y1, int y2, int angle);
void rotate_right_90(image **mat, image **new_mat, int x1, int x2, int y1,
					 int y2);
void rotate_right_180(image **mat, image **new_mat, int x1, int x2, int y1,
					  int y2);
void rotate_right_270(image **mat, image **new_mat, int x1, int x2, int y1,
					  int y2);
void swap_int(int *a, int *b);
void rotate_rectangle(image ***m, int *line, int *col, int *x2, int *y2,
					  int angle, int size);
void rotate_matrix(image ***mat, int x1, int *x2, int y1, int *y2, int size,
				   int *line, int *col);
int exit_and_free(image **mat, int line, int col);
void print_matrix(FILE *g, image **mat, int line, int col, int size, int type,
				  int maxi);
void save_image(image **mat, int line, int col, int size, int maxi);
double clamp(double value);
void equalize(image **mat, int line, int col, int size, int max_value);
double **alloc_matrix(int n, int m);
void free_matrix_double(double **mat, int n);
void apply_matrix(image ***mat, double filter[3][3], int y1, int y2, int x1,
				  int x2, int line, int col);
void apply(image **mat, int line, int col, int size, int y1, int y2, int x1,
		   int x2);
int digit(char *s);
int is_alpha(char c);
int is_sign(char c);
int is_digit_int(int x);
void read_parameters(char *s, char *s1, char *s2, char *s3, char *s4);
void select_all(image **mat, int *x1, int *y1, int *x2, int *y2, int line,
				int col);
void copy_coord(char *s1, char *s2, char *s3, char *s4, int *cpx1, int *cpy1,
				int *cpx2, int *cpy2);

#endif
