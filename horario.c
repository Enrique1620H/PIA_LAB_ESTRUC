#include <stdio.h>
#include <string.h>
#include "horario.h"
#include "usuario.h"

static BloqueHorario horario[HORAS][DIAS];
static const char *NOMBRES_DIAS[DIAS] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

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

static void limpiar_bloque(BloqueHorario *b) {
    b->ocupado = 0;
    b->materia[0] = '\0';
    b->salon[0] = '\0';
}

static void mostrar_horario(void) {
    printf("\nHORARIO SEMANAL\n");
    printf("%-6s", "Hora");
    for (int d = 0; d < DIAS; d++) {
        printf("| %-12s", NOMBRES_DIAS[d]);
    }
    printf("\n");

    for (int h = 0; h < HORAS; h++) {
        printf("%02d:00 ", h + 7);
        for (int d = 0; d < DIAS; d++) {
            if (horario[h][d].ocupado) {
                printf("| %-12.12s", horario[h][d].materia);
            } else {
                printf("| %-12s", "---");
            }
        }
        printf("\n");
    }
}

static void agregar_clase(void) {
    char materia[50], salon[20];

    int dia = leer_int("\nDia (1=Lun, 2=Mar, 3=Mie, 4=Jue, 5=Vie, 6=Sab): ", 1, DIAS);
    int hora_ini = leer_int("Hora inicio (formato de 24h): ", 7, 21);
    int hora_fin = leer_int("Hora fin (formato de 24h): ", hora_ini + 1, 22);
    int d_idx = dia - 1;
    for (int h = hora_ini - 7; h < hora_fin - 7; h++) {
        if (horario[h][d_idx].ocupado) {
            printf("EMPALME DETECTADO: El %s a las %02d:00 ya tienes '%s'.\n",
                   NOMBRES_DIAS[d_idx], h + 7, horario[h][d_idx].materia);
            return;
        }
    }

    printf("Nombre de la materia: ");
    fgets(materia, sizeof(materia), stdin);
    materia[strcspn(materia, "\n")] = '\0';

    printf("Salon o Aula: ");
    fgets(salon, sizeof(salon), stdin);
    salon[strcspn(salon, "\n")] = '\0';

    for (int h = hora_ini - 7; h < hora_fin - 7; h++) {
        horario[h][d_idx].ocupado = 1;
        strncpy(horario[h][d_idx].materia, materia, sizeof(horario[h][d_idx].materia) - 1);
        horario[h][d_idx].materia[sizeof(horario[h][d_idx].materia) - 1] = '\0';
        strncpy(horario[h][d_idx].salon, salon, sizeof(horario[h][d_idx].salon) - 1);
        horario[h][d_idx].salon[sizeof(horario[h][d_idx].salon) - 1] = '\0';
    }
    printf("La clase ha sido agregada correctamente.\n");
}

static void editar_clase(void) {
    int dia = leer_int("\nIngrese dia (1-6) a editar: ", 1, DIAS);
    int hora = leer_int("Ingrese hora (7-21) a editar: ", 7, 21);

    BloqueHorario *b = &horario[hora - 7][dia - 1];
    if (!b->ocupado) {
        printf("Ese bloque esta vacio usar 'Agregar clase'.\n");
        return;
    }

    printf("Materia actual: %s | Nuevo nombre: ", b->materia);
    fgets(b->materia, sizeof(b->materia), stdin);
    b->materia[strcspn(b->materia, "\n")] = '\0';

    printf("Salon actual: %s | Nuevo salon: ", b->salon);
    fgets(b->salon, sizeof(b->salon), stdin);
    b->salon[strcspn(b->salon, "\n")] = '\0';

    printf("Bloque actualizado.\n");
}

static void eliminar_clase(void) {
    int dia = leer_int("\nIngrese dia (1-6) del bloque a eliminar: ", 1, DIAS);
    int hora = leer_int("Ingrese hora (7-21) del bloque a eliminar: ", 7, 21);

    BloqueHorario *b = &horario[hora - 7][dia - 1];
    if (!b->ocupado) {
        printf("Ese bloque ya estaba libre.\n");
        return;
    }

    limpiar_bloque(b);
    printf("Clase eliminada del horario.\n");
}

void exportar_horario_txt(void) {
    FILE *f = fopen(ruta_usuario("reporte_horario.txt"), "w");
    if (!f) return;

    fprintf(f, "REPORTE DE HORARIO SEMANAL\n==========================\n");
    for (int d = 0; d < DIAS; d++) {
        fprintf(f, "\n--- %s ---\n", NOMBRES_DIAS[d]);
        for (int h = 0; h < HORAS; h++) {
            if (horario[h][d].ocupado) {
                fprintf(f, "%02d:00 - %02d:00 | %s (Salon: %s)\n",
                        h + 7, h + 8, horario[h][d].materia, horario[h][d].salon);
            }
        }
    }
    fclose(f);
    printf("Reporte ha sido exportado a '%s'\n", ruta_usuario("reporte_horario.txt"));
}

void guardar_horario(void) {
    FILE *f = fopen(ruta_usuario("horario.dat"), "wb");
    if (!f) return;
    fwrite(horario, sizeof(BloqueHorario), HORAS * DIAS, f);
    fclose(f);
}

void cargar_horario(void) {
    memset(horario, 0, sizeof(horario)); // limpia el horario del usuario anterior (cambio de sesion)
    FILE *f = fopen(ruta_usuario("horario.dat"), "rb");
    if (!f) return;
    fread(horario, sizeof(BloqueHorario), HORAS * DIAS, f);
    fclose(f);
}

void menu_horario(void) {
    int op;
    do {
        printf("\n||MENU HORARIO||\n");
        printf("1.Visualizar tu horario semanal\n");
        printf("2.Agregar clase\n");
        printf("3.Editar clase\n");
        printf("4.Eliminar clase\n");
        printf("5.Exportar horario a .txt\n");
        printf("0.Regresar al menu principal\n");
        op = leer_int("Opcion: ", 0, 5);

        switch (op) {
            case 1: mostrar_horario(); break;
            case 2: agregar_clase(); guardar_horario(); break;
            case 3: editar_clase(); guardar_horario(); break;
            case 4: eliminar_clase(); guardar_horario(); break;
            case 5: exportar_horario_txt(); break;
        }
    } while (op != 0);
}
