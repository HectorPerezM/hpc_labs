#include "mandelbrot.h"

/*
 * Entrada: Puntero a Data
 * Salida: Vacía
 * Obj: Libera la memoria de las filas de data->display 
*/
void clean(Data *data) {
    int i;
    for (i = 0; i < data->row; i++) {
        double *currentPtr = data->display[i];
        free(currentPtr);
    }
}

/*
 * Entrada: Puntero a Data, Puntero a char con nombre del archivo ingresado por parámetro
 * Salida: Vacía
 * Obj: Primero se crea un puntero al archivo de salida, luego se itera sobre data->display
 *      para escribir por fila en el archivo de salida. Finalmente se cierra el archivo.
*/
void writeData(Data *data, char *fileName) {
    FILE *fp = NULL;
    int i = 0;
    
    fp = fopen(fileName, "wb");
    if (fp == NULL) {
        printf("No se puede escribir el archivo: %s", fileName);
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < data->row; i++)
        fwrite(data->display[i], sizeof(double), data->column, fp);
    
    fclose(fp);
}

/*
 * Entrada: Puntero a Data, Entero que representa la profundidad, Double con Cota inferior real, Double con Cota Superior imaginaria, Double con muestreo
 * Salida: Puntero a Data
 * Obj: Primero se realizan dos ciclos para recorrer data->display, luego se calcula el complejo que 
 *      corresponde a esa posicion en la matriz considerando el muestreo. Luego se inician las variables
 *      z a utilizar. Luego se realiza el algoritmo de mandelbrot, en donde se consulta si se cumplen
 *      las condiciones necesarias, luego actualizamos en la matriz display una vez que el valor de interaciones
 *      fue calculado.
*/
Data* mandelbrot(Data *data, int depth, double a, double d, double s) {
    int i,j;

    /*  desde el punto complejo (-1, 1) hasta (1, -1) 
        ,es decir, recorro la columna y luego las filas 
    */

    for (i = 0; i < data->row; i++) {
        for (j = 0; j < data->column; j++) {
            /* calculate complex values */
            int n = 1;

            double c_real = a + (j*s);
            double c_imag = d - (i*s);

            double z_real = 0.0;
            double z_imag = 0.0;

            z_real = 0.0 + c_real;
            z_imag = 0.0 + c_imag;
            do {
                double temp = ((pow(z_real, 2.0)) - (pow(z_imag, 2.0))) + c_real;
                z_imag = (2 * z_real * z_imag) + c_imag;
                z_real = temp;
                n++;
            } while(((pow(z_real, 2.0) + pow(z_imag, 2.0)) < 4.0) && n < depth);
            data->display[i][j] = log((double) n);
        }
    }

    return data;
}

/* 
 * Entrada: Puntero a Data, Parámetros de cotas reales e imaginarioas, y muestreo
 * Salida: Puntero a Data
 * Obj: Primero se calcula la dimension de filas y columnas a utilizar, luego se asigna
 *      memoria a la matriz que contiene la salida de mandelbrot. Finalmente, se retorna
 *      este puntero a Data.
*/
Data* initDataStructure(Data *data, double a, double b, double c, double d, double s) {
    int i;
    data = (Data*)malloc(sizeof(Data));

    /* calculate dimension of complex grid NxM */
    data->column = (int) ((abs(c) + abs(a))/s) + 1;
    data->row    = (int) ((abs(d) + abs(b))/s) + 1;
    printf("Dimension: %d filas %d columnas \n", data->row, data->column);

    data->display = (double**)calloc(data->column, sizeof(double*));
    if (data->display != NULL) {
        for (i = 0; i < data->row; i++) {
            data->display[i] = (double*)calloc(data->column, sizeof(double));
            if (data->display[i] == NULL) {
                perror("Error allocating memory for the display matrix: ");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        perror("Error allocating memory for the display matrix: ");
        exit(EXIT_FAILURE);
    }
    return data;
}

/*
 * Entrada: Recibe parámetros ingresados mediante getopt
 * Salida: Vacía
 * Obj: Primero se crea un puntero a la estructura Data, que contiene la informacion de filas, columnas 
 *      y un puntero doble a double que almacena los valores obtenidos del calculo mandelbrot. Se 
 *      inicializa los valores de Data mediante initDataStructure(), luego se llama a la funcion
 *      mandelbrot() que realiza el calculo y finalmente se escribe los valores obtenidos por mandelbrot
 *      que se encuentran en data->display. Luego, se libera la memoria utilizada por la estructura Data.
*/
void start(int depth, double a, double b, double c, double d, double s, char *fileName) {
    Data *data = NULL;

    data = initDataStructure(data, a, b, c, d, s); 
    data = mandelbrot(data, depth, a, d, s);
    writeData(data, fileName);
    clean(data);
}

/*
 *  Bloque main, recibe los parametros mediante getopt:
 *      -a -> cota inferior real
 *      -b -> cota inferior imaginario
 *      -c -> cota superior real
 *      -d -> cota superior imaginario
 *      -s -> factor de muestreo
 *      -f -> archivo de salida en formato .raw
 * 
 *  Luego de recibir los parametros y asignarlos, verifica si las cotas
 *  corresponden cumplen que los valores superiores sean mayores a los inferiores
 *  y llama a la función start().
 * 
*/
int main(int argc, char **argv) {
    int i = 1;
    double s = 0.1, a = -1.0, b = -1.0, c = 1.0, d = 1.0;
    char *f;

    int opt;
    extern int optopt;
    extern char* optarg;
    
    while((opt = getopt(argc, argv, "i:a:b:c:d:s:f:")) != -1) {
        switch (opt) {
        case 'i':
            sscanf(optarg, "%d", &i);
            if (i <= 0) {
                printf("La bandera -i no puede ser menor o igual a cero.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'a':
            sscanf(optarg, "%lf", &a);
            break;
        case 'b':
            sscanf(optarg, "%lf", &b);
            break;
        case 'c':
            sscanf(optarg, "%lf", &c);
            break;
        case 'd':
            sscanf(optarg, "%lf", &d);
            break;
        case 's':
            sscanf(optarg, "%lf", &s);
            if (s <= 0.0) {
                printf("La bandera -s no puede ser menor o igual a cero.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'f':
            f = optarg;
            break;
        case '?':
            if(optopt == 'c')
            fprintf(stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint(optopt))
            fprintf(stderr, "Opción desconocida '-%c'.\n", optopt);
            else
            fprintf(stderr, "Opción con caracter desconocido. '\\x%x'.\n", optopt);
            exit(EXIT_FAILURE);
        default:
            printf("Faltan argumentos.\n");
            exit(EXIT_FAILURE);
        }       
    }

    /* Comprobando cotas */
    if(c < a) {
        printf("The -c parameter can't be less than -a\n");
        exit(EXIT_FAILURE);
    }

    if(d < b) {
        printf("The -d parameter can't be less than -b\n");
        exit(EXIT_FAILURE);
    }

    start(i,a,b,c,d,s,f);
    return EXIT_SUCCESS;
}