#ifndef POMODORO_H // Evita incluir este encabezado mas de una vez al compilar
#define POMODORO_H


void menu_pomodoro(void);    // Abre el submenu de temporizador, historial (CRUD) y estadisticas
float horas_estudiadas(void); // Devuelve el total de horas acumuladas para el resumen semanal
void guardar_estudio(void);  // Guarda las sesiones de estudio en archivo binario
void cargar_estudio(void);   // Carga las sesiones guardadas al iniciar el programa


void liberar_estudio(void);

#endif