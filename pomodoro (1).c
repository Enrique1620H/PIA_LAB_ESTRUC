#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <time.h>  
#include "pomodoro.h" // Incluye las declaraciones publicas de este modulo
#include "usuario.h"

#define ARCHIVO_ESTUDIO ruta_usuario("estudio.dat") // Archivo binario donde se guardan las sesiones

// Estructura que representa una sesion de estudio o Pomodoro completado
typedef struct {
    int id;          // Identificador unico para editar o eliminar una sesion exacta
    char materia[30]; 
    int minutos;     
    char nota[80];   
} SesionEstudio;

static SesionEstudio *sesiones = NULL; // Arreglo dinamico de sesiones de estudio
static int num_sesiones = 0;           
static int capacidad_sesiones = 0;     
static int siguiente_id = 1;      


static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


static void leer_texto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    if (fgets(dest, tam, stdin) != NULL) {
        dest[strcspn(dest, "\n")] = '\0';
    }
}


static int leer_rango(const char *msg, int min, int max) {
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


static int asegurar_capacidad(void) {
    if (num_sesiones >= capacidad_sesiones) {
        // Duplicamos la capacidad (empezando en 4) para amortizar reasignaciones
        int nueva_cap = (capacidad_sesiones == 0) ? 4 : capacidad_sesiones * 2;
        SesionEstudio *temp = (SesionEstudio *)realloc(sesiones, nueva_cap * sizeof(SesionEstudio));
        if (temp == NULL) {
            printf("Error: sin memoria disponible para registrar la sesion.\n");
            return 0;
        }
        sesiones = temp;
        capacidad_sesiones = nueva_cap;
    }
    return 1;
}

// Busca el indice de una sesion por su ID; devuelve -1 si no existe
static int buscar_indice_por_id(int id) {
    for (int i = 0; i < num_sesiones; i++) {
        if (sesiones[i].id == id) {
            return i;
        }
    }
    return -1;
}


static void ver_sesiones(void) {
    if (num_sesiones == 0) {
        printf("\nNo hay sesiones de estudio registradas.\n");
        return;
    }

    printf("\n--- HISTORIAL DE SESIONES DE ESTUDIO ---\n");
    for (int i = 0; i < num_sesiones; i++) {
        printf("#%d | Materia: %s | Duracion: %d min (%.2f hrs) | Nota: %s\n",
               sesiones[i].id,
               sesiones[i].materia,
               sesiones[i].minutos,
               sesiones[i].minutos / 60.0f,
               sesiones[i].nota);
    }
}

// Ejecuta un temporizador Pomodoro en tiempo real y guarda la sesion al terminar
static void iniciar_pomodoro(void) {
    char materia[30];
    leer_texto("Materia a estudiar: ", materia, sizeof(materia));
    int minutos = leer_rango("Duracion del Pomodoro en minutos (ej. 25, o 1 para probar): ", 1, 240);

    int segundos_totales = minutos * 60;
    time_t inicio = time(NULL);
    time_t ultimo_segundo = -1;

    printf("\nPomodoro iniciado (%d min) para '%s'.\n", minutos, materia);
    printf("(Presiona Ctrl+C si necesitas detenerlo antes de tiempo)\n");

    // Bucle de cuenta regresiva basado en el reloj real del sistema
    while (1) {
        time_t ahora = time(NULL);
        int transcurrido = (int)difftime(ahora, inicio);
        int restante = segundos_totales - transcurrido;

        if (restante <= 0) {
            break;
        }

        // Actualiza la pantalla solo cuando cambia el segundo para no saturar la consola
        if (ahora != ultimo_segundo) {
            ultimo_segundo = ahora;
            printf("\rTiempo restante: %02d:%02d ", restante / 60, restante % 60);
            fflush(stdout);
        }
    }

    printf("\rTiempo restante: 00:00 - Tiempo completado!\n");

    if (!asegurar_capacidad()) return;

    sesiones[num_sesiones].id = siguiente_id++;
    strcpy(sesiones[num_sesiones].materia, materia);
    sesiones[num_sesiones].minutos = minutos;
    leer_texto("Nota breve de lo estudiado: ", sesiones[num_sesiones].nota, sizeof(sesiones[num_sesiones].nota));
    num_sesiones++;

    guardar_estudio();
    printf("Sesion guardada en el historial.\n");
}

// Agrega una sesion de estudio manualmente sin esperar el temporizador
static void agregar_sesion_manual(void) {
    if (!asegurar_capacidad()) return;

    SesionEstudio *s = &sesiones[num_sesiones];
    s->id = siguiente_id++;
    leer_texto("Materia: ", s->materia, sizeof(s->materia));
    s->minutos = leer_rango("Minutos estudiados (1 a 1440): ", 1, 1440);
    leer_texto("Nota o tema repasado: ", s->nota, sizeof(s->nota));
    num_sesiones++;

    guardar_estudio();
    printf("Sesion #%d agregada.\n", s->id);
}

// Edita una sesion existente (materia, minutos o nota) buscandola por su ID
static void editar_sesion(void) {
    if (num_sesiones == 0) {
        printf("\nNo hay sesiones para editar.\n");
        return;
    }

    ver_sesiones();
    int id = leer_rango("\nID de la sesion a editar (0 para cancelar): ", 0, siguiente_id - 1);
    if (id == 0) return;

    int idx = buscar_indice_por_id(id);
    if (idx == -1) {
        printf("No se encontro la sesion #%d.\n", id);
        return;
    }

    printf("\nEditando sesion #%d (actual: %s, %d min):\n",
           sesiones[idx].id, sesiones[idx].materia, sesiones[idx].minutos);
    printf("1. Editar materia\n");
    printf("2. Editar duracion en minutos\n");
    printf("3. Editar nota\n");
    int op = leer_rango("Opcion: ", 1, 3);

    switch (op) {
        case 1:
            leer_texto("Nueva materia: ", sesiones[idx].materia, sizeof(sesiones[idx].materia));
            break;
        case 2:
            sesiones[idx].minutos = leer_rango("Nuevos minutos (1 a 1440): ", 1, 1440);
            break;
        case 3:
            leer_texto("Nueva nota: ", sesiones[idx].nota, sizeof(sesiones[idx].nota));
            break;
    }

    guardar_estudio();
    printf("Sesion #%d actualizada.\n", id);
}

// Elimina una sesion por su ID y recorre los elementos posteriores para mantener el arreglo contiguo
static void eliminar_sesion(void) {
    if (num_sesiones == 0) {
        printf("\nNo hay sesiones para eliminar.\n");
        return;
    }

    ver_sesiones();
    int id = leer_rango("\nID de la sesion a eliminar (0 para cancelar): ", 0, siguiente_id - 1);
    if (id == 0) return;

    int idx = buscar_indice_por_id(id);
    if (idx == -1) {
        printf("No se encontro la sesion #%d.\n", id);
        return;
    }

    for (int i = idx; i < num_sesiones - 1; i++) {
        sesiones[i] = sesiones[i + 1];
    }
    num_sesiones--;

    guardar_estudio();
    printf("Sesion #%d eliminada.\n", id);
}

// Calcula y muestra las horas por materia, ademas de cual estudiaste mas y cual menos
static void mostrar_estadisticas(void) {
    if (num_sesiones == 0) {
        printf("\nAun no hay sesiones registradas para calcular estadisticas.\n");
        return;
    }

    // Arreglos paralelos temporales para acumular minutos por cada materia distinta
    char materias[200][30];
    int minutos_materia[200] = {0};
    int distintas = 0;

    for (int i = 0; i < num_sesiones; i++) {
        int pos = -1;
        for (int j = 0; j < distintas; j++) {
            if (strcmp(materias[j], sesiones[i].materia) == 0) {
                pos = j;
                break;
            }
        }

        if (pos != -1) {
            minutos_materia[pos] += sesiones[i].minutos;
        } else if (distintas < 200) {
            strcpy(materias[distintas], sesiones[i].materia);
            minutos_materia[distintas] = sesiones[i].minutos;
            distintas++;
        }
    }

    int idx_max = 0;
    int idx_min = 0;

    printf("\n--- ESTADISTICAS DE ESTUDIO ---\n");
    for (int i = 0; i < distintas; i++) {
        printf("- %s: %.2f hrs (%d min)\n",
               materias[i], minutos_materia[i] / 60.0f, minutos_materia[i]);
        if (minutos_materia[i] > minutos_materia[idx_max]) idx_max = i;
        if (minutos_materia[i] < minutos_materia[idx_min]) idx_min = i;
    }

    printf("\nTotal estudiado: %.2f horas\n", horas_estudiadas());
    printf("Materia MAS estudiada:   %s (%.2f hrs)\n", materias[idx_max], minutos_materia[idx_max] / 60.0f);
    printf("Materia MENOS estudiada: %s (%.2f hrs)\n", materias[idx_min], minutos_materia[idx_min] / 60.0f);
}

// Submenu interactivo del modulo de Pomodoro y registro de estudio
void menu_pomodoro(void) {
    int op;
    do {
        printf("\n=== MODULO POMODORO Y ESTUDIO ===\n");
        printf("1. Iniciar temporizador Pomodoro\n");
        printf("2. Agregar sesion manualmente\n");
        printf("3. Ver historial de sesiones\n");
        printf("4. Editar sesion\n");
        printf("5. Eliminar sesion\n");
        printf("6. Ver estadisticas (mas y menos estudiada)\n");
        printf("0. Volver\n");
        op = leer_rango("Opcion: ", 0, 6);

        switch (op) {
            case 1: iniciar_pomodoro(); break;
            case 2: agregar_sesion_manual(); break;
            case 3: ver_sesiones(); break;
            case 4: editar_sesion(); break;
            case 5: eliminar_sesion(); break;
            case 6: mostrar_estadisticas(); break;
            case 0: break;
        }
    } while (op != 0);
}

// Suma todos los minutos registrados y los devuelve convertidos en horas
float horas_estudiadas(void) {
    int total_minutos = 0;
    for (int i = 0; i < num_sesiones; i++) {
        total_minutos += sesiones[i].minutos;
    }
    return total_minutos / 60.0f;
}

// Guarda el total de sesiones y el arreglo completo en un archivo binario
void guardar_estudio(void) {
    FILE *f = fopen(ARCHIVO_ESTUDIO, "wb");
    if (f == NULL) {
        printf("No se pudo guardar '%s'.\n", ARCHIVO_ESTUDIO);
        return;
    }

    fwrite(&num_sesiones, sizeof(int), 1, f);
    if (num_sesiones > 0 && sesiones != NULL) {
        fwrite(sesiones, sizeof(SesionEstudio), num_sesiones, f);
    }

    fclose(f);
}

// Carga las sesiones desde el archivo binario al abrir el programa
void cargar_estudio(void) {
    liberar_estudio(); // Evita duplicar datos o fugas si se invoca mas de una vez

    FILE *f = fopen(ARCHIVO_ESTUDIO, "rb");
    if (f == NULL) return; // Primera ejecucion: aun no existe el archivo

    int cantidad = 0;
    if (fread(&cantidad, sizeof(int), 1, f) != 1 || cantidad <= 0) {
        fclose(f);
        return;
    }

    sesiones = (SesionEstudio *)malloc(cantidad * sizeof(SesionEstudio));
    if (sesiones == NULL) {
        fclose(f);
        return;
    }

    num_sesiones = (int)fread(sesiones, sizeof(SesionEstudio), cantidad, f);
    capacidad_sesiones = num_sesiones;
    siguiente_id = 1;

    for (int i = 0; i < num_sesiones; i++) {
        if (sesiones[i].id >= siguiente_id) {
            siguiente_id = sesiones[i].id + 1;
        }
    }

    fclose(f);
}

// Libera la memoria dinamica del arreglo de sesiones y reinicia los contadores
void liberar_estudio(void) {
    free(sesiones);
    sesiones = NULL;
    num_sesiones = 0;
    capacidad_sesiones = 0;
    siguiente_id = 1;
}