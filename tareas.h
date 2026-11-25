#ifndef TAREAS_H // Evita incluir este archivo dos veces por error al compilar
#define TAREAS_H

// Funciones requeridas del modulo de tareas
void menu_tareas(void);    // Abre el menu para ver, agregar, editar y eliminar tareas
void resumen_tareas(void); // Muestra un resumen rapido para el resumen semanal
void guardar_tareas(void); // Guarda las tareas en archivo binario
void cargar_tareas(void);  // Carga las tareas desde archivo binario al iniciar

// Funcion para liberar la memoria dinamica de la lista enlazada
void liberar_tareas(void);

#endif