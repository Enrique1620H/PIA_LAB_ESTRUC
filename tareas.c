#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <time.h>    
#include "tareas.h"  // Incluye las declaraciones publicas de este modulo
#include "usuario.h"

#define ARCHIVO_TAREAS ruta_usuario("tareas.dat") // Nombre del archivo binario de tareas

// Estructura de cada nodo en la lista enlazada
typedef struct Tarea {
    int id;                  // Identificador unico para editar o eliminar sin equivocarte
    char titulo[80];         
    char materia[30];        
    int fecha_limite;        // Fecha en formato AAAAMMDD 
    int prioridad;           // 1 = Baja, 2 = Media, 3 = Alta
    int estado;              // 0 = Pendiente, 1 = Hecha
    struct Tarea *siguiente; // Puntero al siguiente nodo de la lista
} Tarea;

// Estructura plana para guardar y leer registros limpios en binario
typedef struct {
    int id;
    char titulo[80];
    char materia[30];
    int fecha_limite;
    int prioridad;
    int estado;
} TareaArchivo;

static Tarea *cabeza = NULL; // Puntero al primer nodo de la lista enlazada
static int siguiente_id = 1; // Contador para asignar IDs unicos a cada tarea nueva


static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Lee un texto con espacios de forma segura y borra el salto de linea '\n' final
static void leer_texto(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    if (fgets(dest, tam, stdin) != NULL) {
        dest[strcspn(dest, "\n")] = '\0';
    }
}

// Pide un numero entero dentro de [min, max] hasta que el usuario ingrese un dato valido
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

// Obtiene la fecha de hoy del sistema en formato AAAAMMDD
static int hoy_aaaammdd(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    return (tm_info->tm_year + 1900) * 10000 +
           (tm_info->tm_mon + 1) * 100 +
           tm_info->tm_mday;
}

// Convierte una fecha AAAAMMDD a tiempo a medianoche para calcular diferencia real en dias
static time_t a_medianoche(int fecha) {
    struct tm t = {0};
    t.tm_year = (fecha / 10000) - 1900;
    t.tm_mon = ((fecha / 100) % 100) - 1;
    t.tm_mday = fecha % 100;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    return mktime(&t);
}

// Calcula cuantos dias faltan para la fecha limite (negativo si ya vencio)
static int dias_para_vencer(int fecha_limite) {
    double seg = difftime(a_medianoche(fecha_limite), a_medianoche(hoy_aaaammdd()));
    return (int)(seg / 86400.0);
}

// Muestra una fecha AAAAMMDD en formato DD/MM/AAAA
static void imprimir_fecha(int fecha) {
    printf("%02d/%02d/%04d", fecha % 100, (fecha / 100) % 100, fecha / 10000);
}

// Inserta un nodo en la posicion correcta para mantener la lista ordenada por fecha limite
static void insertar_ordenado(Tarea *nuevo) {
    nuevo->siguiente = NULL;

    // Si la lista esta vacia o su fecha va antes que la primera tarea
    if (cabeza == NULL || nuevo->fecha_limite < cabeza->fecha_limite) {
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        return;
    }

    // Recorre hasta encontrar donde queda ordenado por fecha_limite
    Tarea *actual = cabeza;
    while (actual->siguiente != NULL && actual->siguiente->fecha_limite <= nuevo->fecha_limite) {
        actual = actual->siguiente;
    }

    nuevo->siguiente = actual->siguiente;
    actual->siguiente = nuevo;
}

// Busca una tarea por su ID y devuelve su puntero (o NULL si no existe)
static Tarea* buscar_tarea(int id) {
    Tarea *actual = cabeza;
    while (actual != NULL) {
        if (actual->id == id) return actual;
        actual = actual->siguiente;
    }
    return NULL;
}

// Imprime todas las tareas ordenadas por fecha limite y muestra alertas de lo que vence pronto
static void ver_tareas(void) {
    if (cabeza == NULL) {
        printf("\nNo hay tareas registradas.\n");
        return;
    }

    const char *prio[] = {"", "Baja", "Media", "Alta"};
    printf("\n--- TAREAS Y ENTREGAS (ORDENADAS POR FECHA LIMITE) ---\n");

    Tarea *actual = cabeza;
    while (actual != NULL) {
        printf("#%d | %s | %s | Limite: ", actual->id, actual->titulo, actual->materia);
        imprimir_fecha(actual->fecha_limite);
        printf(" | Prioridad: %s | Estado: %s",
               prio[actual->prioridad],
               actual->estado == 1 ? "Hecha" : "Pendiente");

        // Alertas solo para tareas pendientes
        if (actual->estado == 0) {
            int dias = dias_para_vencer(actual->fecha_limite);
            if (dias < 0) {
                printf("  [ALERTA: VENCIDA hace %d dia(s)]", -dias);
            } else if (dias == 0) {
                printf("  [ALERTA: VENCE HOY]");
            } else if (dias <= 3) {
                printf("  [ALERTA: VENCE PRONTO en %d dia(s)]", dias);
            }
        }
        printf("\n");
        actual = actual->siguiente;
    }
}

// Agrega una nueva tarea a la lista enlazada en orden de entrega y guarda en binario
static void agregar_tarea(void) {
    Tarea *nueva = (Tarea *)malloc(sizeof(Tarea));
    if (nueva == NULL) {
        printf("Sin memoria para crear la tarea.\n");
        return;
    }

    nueva->id = siguiente_id++;
    leer_texto("Titulo: ", nueva->titulo, sizeof(nueva->titulo));
    leer_texto("Materia: ", nueva->materia, sizeof(nueva->materia));
    nueva->fecha_limite = leer_rango("Fecha limite (AAAAMMDD, ej. 20261125): ", 19000101, 29991231);
    nueva->prioridad = leer_rango("Prioridad (1=Baja, 2=Media, 3=Alta): ", 1, 3);
    nueva->estado = 0; // Inicia como Pendiente

    insertar_ordenado(nueva);
    guardar_tareas();
    printf("Tarea #%d agregada.\n", nueva->id);
}

// Edita una tarea existente (estado, titulo, materia, fecha limite o prioridad)
static void editar_tarea(void) {
    if (cabeza == NULL) {
        printf("\nNo hay tareas para editar.\n");
        return;
    }

    ver_tareas();
    int id = leer_rango("\nID de tarea a editar (0 para cancelar): ", 0, siguiente_id - 1);
    if (id == 0) return;

    Tarea *t = buscar_tarea(id);
    if (t == NULL) {
        printf("No se encontro la tarea #%d.\n", id);
        return;
    }

    printf("\nEditando '%s':\n", t->titulo);
    printf("1. Cambiar estado (Pendiente / Hecha)\n");
    printf("2. Editar titulo\n");
    printf("3. Editar materia\n");
    printf("4. Editar fecha limite\n");
    printf("5. Editar prioridad\n");
    int op = leer_rango("Opcion: ", 1, 5);

    switch (op) {
        case 1:
            t->estado = (t->estado == 0) ? 1 : 0;
            printf("Estado actualizado a: %s\n", t->estado == 1 ? "Hecha" : "Pendiente");
            break;
        case 2:
            leer_texto("Nuevo titulo: ", t->titulo, sizeof(t->titulo));
            break;
        case 3:
            leer_texto("Nueva materia: ", t->materia, sizeof(t->materia));
            break;
        case 4: {
            t->fecha_limite = leer_rango("Nueva fecha limite (AAAAMMDD): ", 19000101, 29991231);

            // Desenlaza el nodo y lo vuelve a insertar para conservar el orden por fecha
            if (cabeza == t) {
                cabeza = t->siguiente;
            } else {
                Tarea *prev = cabeza;
                while (prev->siguiente != NULL && prev->siguiente != t) {
                    prev = prev->siguiente;
                }
                if (prev->siguiente == t) {
                    prev->siguiente = t->siguiente;
                }
            }
            insertar_ordenado(t);
            break;
        }
        case 5:
            t->prioridad = leer_rango("Nueva prioridad (1=Baja, 2=Media, 3=Alta): ", 1, 3);
            break;
    }

    guardar_tareas();
    printf("Tarea #%d editada.\n", id);
}

// Elimina una tarea por su ID y libera su memoria con free()
static void eliminar_tarea(void) {
    if (cabeza == NULL) {
        printf("\nNo hay tareas para eliminar.\n");
        return;
    }

    ver_tareas();
    int id = leer_rango("\nID de tarea a eliminar (0 para cancelar): ", 0, siguiente_id - 1);
    if (id == 0) return;

    Tarea *actual = cabeza;
    Tarea *anterior = NULL;

    while (actual != NULL && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == NULL) {
        printf("No se encontro la tarea #%d.\n", id);
        return;
    }

    // Quita el nodo de la lista enlazada
    if (anterior == NULL) {
        cabeza = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    free(actual);
    guardar_tareas();
    printf("Tarea #%d eliminada.\n", id);
}


void menu_tareas(void) {
    int op;
    do {
        printf("\n=== MODULO DE TAREAS ===\n");
        printf("1. Ver tareas y alertas\n");
        printf("2. Agregar tarea\n");
        printf("3. Editar tarea\n");
        printf("4. Eliminar tarea\n");
        printf("0. Volver\n");
        op = leer_rango("Opcion: ", 0, 4);

        switch (op) {
            case 1: ver_tareas(); break;
            case 2: agregar_tarea(); break;
            case 3: editar_tarea(); break;
            case 4: eliminar_tarea(); break;
            case 0: break;
        }
    } while (op != 0);
}

// Muestra un resumen de tareas para el resumen semanal
void resumen_tareas(void) {
    int total = 0, pendientes = 0, hechas = 0, urgentes = 0;
    Tarea *actual = cabeza;

    while (actual != NULL) {
        total++;
        if (actual->estado == 1) {
            hechas++;
        } else {
            pendientes++;
            if (dias_para_vencer(actual->fecha_limite) <= 3) {
                urgentes++;
            }
        }
        actual = actual->siguiente;
    }

    printf("Tareas: %d (%d hechas, %d pendientes, %d vencidas o por vencer en <=3 dias)\n",
           total, hechas, pendientes, urgentes);
}

// Guarda toda la lista enlazada en el archivo binario
void guardar_tareas(void) {
    FILE *f = fopen(ARCHIVO_TAREAS, "wb");
    if (f == NULL) {
        printf("No se pudo guardar '%s'.\n", ARCHIVO_TAREAS);
        return;
    }

    int total = 0;
    Tarea *actual = cabeza;
    while (actual != NULL) {
        total++;
        actual = actual->siguiente;
    }

    fwrite(&total, sizeof(int), 1, f);

    actual = cabeza;
    while (actual != NULL) {
        TareaArchivo reg;
        reg.id = actual->id;
        strcpy(reg.titulo, actual->titulo);
        strcpy(reg.materia, actual->materia);
        reg.fecha_limite = actual->fecha_limite;
        reg.prioridad = actual->prioridad;
        reg.estado = actual->estado;
        fwrite(&reg, sizeof(TareaArchivo), 1, f);
        actual = actual->siguiente;
    }

    fclose(f);
}

// Carga las tareas desde el archivo binario y reconstruye la lista ordenada
void cargar_tareas(void) {
    liberar_tareas();

    FILE *f = fopen(ARCHIVO_TAREAS, "rb");
    if (f == NULL) return; // Primera vez que se abre el programa

    int total = 0;
    if (fread(&total, sizeof(int), 1, f) != 1 || total <= 0) {
        fclose(f);
        return;
    }

    siguiente_id = 1;
    for (int i = 0; i < total; i++) {
        TareaArchivo reg;
        if (fread(&reg, sizeof(TareaArchivo), 1, f) != 1) break;

        Tarea *nodo = (Tarea *)malloc(sizeof(Tarea));
        if (nodo == NULL) break;

        nodo->id = reg.id;
        strcpy(nodo->titulo, reg.titulo);
        strcpy(nodo->materia, reg.materia);
        nodo->fecha_limite = reg.fecha_limite;
        nodo->prioridad = reg.prioridad;
        nodo->estado = reg.estado;
        nodo->siguiente = NULL;

        insertar_ordenado(nodo);

        if (reg.id >= siguiente_id) {
            siguiente_id = reg.id + 1;
        }
    }

    fclose(f);
}

// Libera toda la memoria dinamica ocupada por los nodos de la lista
void liberar_tareas(void) {
    Tarea *actual = cabeza;
    while (actual != NULL) {
        Tarea *sig = actual->siguiente;
        free(actual);
        actual = sig;
    }
    cabeza = NULL;
    siguiente_id = 1;
}
