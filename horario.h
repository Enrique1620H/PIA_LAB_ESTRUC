#ifndef HORARIO_H
#define HORARIO_H

#define DIAS 6   
#define HORAS 15 

typedef struct {
    int ocupado;           
    char materia[50];
    char salon[20];
} BloqueHorario;

void menu_horario(void);
void guardar_horario(void);
void cargar_horario(void);
void exportar_horario_txt(void); 
#endif