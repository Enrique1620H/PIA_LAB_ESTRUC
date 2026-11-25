#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gastos.h"
#include "usuario.h"

#define ARCHIVO_GASTOS ruta_usuario("gastos.dat")
#define REPORTE_GASTOS ruta_usuario("reporte_gastos.txt")

static Gasto *gastos = NULL;
static int total = 0;
static int capacidad = 0;
static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int leer_int(const char *msg, int min, int max) {
    int v;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &v) == 1 && v >= min && v <= max) {
            limpiar_buffer();
            return v;
        }
        limpiar_buffer();
        printf("Valor invalido (%d a %d).\n", min, max);
    }
}

static float leer_float(const char *msg, float min, float max) {
    float v;
    while (1) {
        printf("%s", msg);
        if (scanf("%f", &v) == 1 && v >= min && v <= max) {
            limpiar_buffer();
            return v;
        }
        limpiar_buffer();
        printf("Valor invalido (%.2f a %.2f).\n", min, max);
    }
}

static const char *nombre_categoria(int cat) {
    switch (cat) {
        case CAT_COMIDA: return "Comida";
        case CAT_TRANSPORTE: return "Transporte";
        case CAT_COPIAS: return "Copias/Material";
        default: return "Otros";
    }
}
static void calcular_estadisticas(int mes, int sem, float *tot_sem, float *tot_mes, float por_cat[4]) {
    *tot_sem = 0.0f;
    *tot_mes = 0.0f;
    for (int i = 0; i < 4; i++) por_cat[i] = 0.0f;

    for (int i = 0; i < total; i++) {
        if (gastos[i].mes == mes) {
            *tot_mes += gastos[i].monto;
            int idx = gastos[i].categoria - 1;
            if (idx >= 0 && idx < 4) {
                por_cat[idx] += gastos[i].monto;
            }
            if (gastos[i].semana == sem) {
                *tot_sem += gastos[i].monto;
            }
        }
    }
}

void mostrar_gastos(void) {
    if (total == 0) {
        printf("No existen actualmente gastos registrados.\n");
        return;
    }
    printf("\n#  | Fecha|| Sem|| Categoria      || Monto    || Descripcion|\n");
    for (int i = 0; i < total; i++) {
        printf("%-2d | %02d/%02d | S%d  | %-15s | $%-8.2f | %s\n",
               i + 1, gastos[i].dia, gastos[i].mes, gastos[i].semana,
               nombre_categoria(gastos[i].categoria),
               gastos[i].monto, gastos[i].descripcion);
    }
}

static int elegir_gasto(void) {
    if (total == 0) {
        printf("No existen actualmente gastos registrados.\n");
        return -1;
    }
    mostrar_gastos();
    return leer_int("Numero de gasto: ", 1, total) - 1;
}

static void agregar_gasto(void) {
    if (total == capacidad) {
        int nueva = (capacidad == 0) ? 2 : capacidad * 2;
        Gasto *tmp = realloc(gastos, nueva * sizeof(Gasto));
        if (tmp == NULL) {
            printf("No hay memoria disponible.\n");
            return;
        }
        gastos = tmp;
        capacidad = nueva;
    }

    Gasto *g = &gastos[total];

    printf("Descripcion del gasto: ");
    fgets(g->descripcion, MAX_DESC_GASTO, stdin);
    g->descripcion[strcspn(g->descripcion, "\n")] = '\0';

    g->monto = leer_float("Monto ($): ", 0.01f, 100000.0f);
    g->categoria = leer_int("Categoria (1=Comida, 2=Transporte, 3=Copias, 4=Otros): ", 1, 4);
    g->dia = leer_int("Dia del gasto (1-31): ", 1, 31);
    g->mes = leer_int("Mes del gasto (1-12): ", 1, 12);
    g->semana = ((g->dia - 1) / 7) + 1;

    total++;
    printf("Gasto agregado.\n");
}

static void editar_gasto(void) {
    int i = elegir_gasto();
    if (i < 0) return;

    Gasto *g = &gastos[i];
    printf("Editando gasto '%s' ($%.2f)\n", g->descripcion, g->monto);

    printf("Nueva descripcion: ");
    fgets(g->descripcion, MAX_DESC_GASTO, stdin);
    g->descripcion[strcspn(g->descripcion, "\n")] = '\0';

    g->monto = leer_float("Nuevo monto ($): ", 0.01f, 100000.0f);
    g->categoria = leer_int("Nueva categoria (1=Comida, 2=Transporte, 3=Copias, 4=Otros): ", 1, 4);
    g->dia = leer_int("Nuevo dia (1-31): ", 1, 31);
    g->mes = leer_int("Nuevo mes (1-12): ", 1, 12);
    g->semana = ((g->dia - 1) / 7) + 1;

    printf("Gasto actualizado.\n");
}

static void eliminar_gasto(void) {
    int i = elegir_gasto();
    if (i < 0) return;

    for (int j = i; j < total - 1; j++) {
        gastos[j] = gastos[j + 1];
    }
    total--;
    printf("Gasto eliminado.\n");
}

void resumen_gastos(void) {
    if (total == 0) {
        printf("\n[Gastos] Sin gastos registrados aun.\n");
        return;
    }
    int mes_ult = gastos[total - 1].mes;
    int sem_ult = gastos[total - 1].semana;

    float tot_sem, tot_mes, por_cat[4];
    calcular_estadisticas(mes_ult, sem_ult, &tot_sem, &tot_mes, por_cat);

    printf("\n||RESUMEN DE GASTOS (Mes %02d - Semana %d)||\n", mes_ult, sem_ult);
    printf("Gasto total de la semana %d: $%.2f\n", sem_ult, tot_sem);
    printf("Gasto total del mes %02d:     $%.2f\n", mes_ult, tot_mes);
    printf("Desglose mensual por categoria:\n");
    for (int c = 1; c <= 4; c++) {
        printf("  - %-15s: $%.2f\n", nombre_categoria(c), por_cat[c - 1]);
    }
}

void exportar_gastos_txt(void) {
    FILE *f = fopen(REPORTE_GASTOS, "w");
    if (f == NULL) {
        printf("No se pudo crear el reporte de texto.\n");
        return;
    }
    fprintf(f, "REPORTE DE GASTOS DEL ESTUDIANTE\n");
    float suma_total = 0;
    for (int i = 0; i < total; i++) {
        fprintf(f, "%02d/%02d (Sem %d) | %-15s | $%-8.2f | %s\n",
                gastos[i].dia, gastos[i].mes, gastos[i].semana,
                nombre_categoria(gastos[i].categoria),
                gastos[i].monto, gastos[i].descripcion);
        suma_total += gastos[i].monto;
    }
    fprintf(f, "\nTotal acumulado: $%.2f\n", suma_total);
    fclose(f);
    printf("Reporte exportado a '%s'.\n", REPORTE_GASTOS);
}

void guardar_gastos(void) {
    FILE *f = fopen(ARCHIVO_GASTOS, "wb");
    if (f == NULL) {
        printf("No se pudo guardar gastos.\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, f);
    fwrite(gastos, sizeof(Gasto), total, f);
    fclose(f);
}

void cargar_gastos(void) {
    liberar_gastos(); // limpia lo del usuario anterior (cambio de sesion)
    FILE *f = fopen(ARCHIVO_GASTOS, "rb");
    if (f == NULL) return;
    int n;
    if (fread(&n, sizeof(int), 1, f) != 1 || n < 0) {
        fclose(f);
        return;
    }
    capacidad = (n < 2) ? 2 : n;
    gastos = malloc(capacidad * sizeof(Gasto));
    if (gastos == NULL) {
        capacidad = 0;
        fclose(f);
        return;
    }
    total = (int)fread(gastos, sizeof(Gasto), n, f);
    fclose(f);
}

void liberar_gastos(void) {
    free(gastos);
    gastos = NULL;
    total = 0;
    capacidad = 0;
}

void menu_gastos(void) {
    int op;
    do {
        printf("\n GASTOS DEL ESTUDIANTE \n");
        printf("1.Agregar gasto\n");
        printf("2.Ver gastos\n");
        printf("3.Editar gasto\n");
        printf("4.Eliminar gasto\n");
        printf("5.Ver resumen semanal y mensual\n");
        printf("6.Exportar reporte a .txt\n");
        printf("0.Volver\n");
        op = leer_int("Opcion: ", 0, 6);

        switch (op) {
            case 1: agregar_gasto(); guardar_gastos(); break;
            case 2: mostrar_gastos(); break;
            case 3: editar_gasto(); guardar_gastos(); break;
            case 4: eliminar_gasto(); guardar_gastos(); break;
            case 5: resumen_gastos(); break;
            case 6: exportar_gastos_txt(); break;
        }
    } while (op != 0);
}