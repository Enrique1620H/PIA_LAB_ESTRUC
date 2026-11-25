//Materias.h es el indice del modulo o algo parecido, al integrante que le toque usar el #include "materias.h", le sera de apoyo

#ifndef MATERIAS_H //ifndef, define y endif nos van a ayudar a evitar errores de compilacion por definición cuando quieran incluir el mismo archivo
#define MATERIAS_H

#define MAX_NOMBRE 30 //Aqui se marcan constantes con nombres
#define MAX_PARCIALES 7

typedef struct //usaremos esta estructura para la limpieza del codigo, evitando que tengamos que definir struct todo el tiempo
{
    char nombre[MAX_NOMBRE];
    int num_parciales;
    float calificaciones[MAX_PARCIALES]; //Arreglo fijo donde,  -1 = SIN CALIFICAR
    float ponderaciones[MAX_PARCIALES]; //Es el % de cada parcial
    float pond_final; //% del examen final y al final deben de sumar 100

} Materia;

void menu_materias(void);  //el void lo ocupamos para que solamente anuncien nombre, parametros y su tipo
int total_materias(void);
const char *obtener_nombre_materia(int i);
void guardar_materias(void);
void cargar_materias(void);
void liberar_materias(void);

void agregar_materias(void);
void capturar_calificacion(int indice);
void mostrar_materias(void);
float calcular_acumulado(const Materia *m); 
float calificacion_necesaria(const Materia *m, float meta);
float promedio_materia(const Materia *m);  //promedio de una materia con lo ya calificado (-1 = sin calificaciones)
float promedio_general(void);              //promedio de todas las materias (-1 = ninguna calificada)

#endif