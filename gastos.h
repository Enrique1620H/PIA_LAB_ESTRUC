#ifndef GASTOS_H
#define GASTOS_H

#define MAX_DESC_GASTO 40

typedef enum {
    CAT_COMIDA = 1,
    CAT_TRANSPORTE = 2,
    CAT_COPIAS = 3,
    CAT_OTROS = 4
} CategoriaGasto;

typedef struct {
    char descripcion[MAX_DESC_GASTO];
    float monto;
    int categoria;
    int dia;      
    int mes;     
    int semana;  
} Gasto;
void menu_gastos(void);
void resumen_gastos(void);
void guardar_gastos(void);
void cargar_gastos(void);
void liberar_gastos(void);
void exportar_gastos_txt(void);
#endif