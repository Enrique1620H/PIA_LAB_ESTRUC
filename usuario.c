#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "usuario.h"

static char usuario_actual[50] = "";
static char ruta[128];

// Archivos que genera cada usuario (si agregan otro, ponerlo aqui)
static const char *ARCHIVOS[] = {
    "tareas.dat", "materias.dat", "horario.dat", "estudio.dat", "gastos.dat",
    "reporte_gastos.txt", "reporte_horario.txt"
};
#define NUM_ARCHIVOS (sizeof(ARCHIVOS) / sizeof(ARCHIVOS[0]))

void establecer_usuario(const char *nombre) {
    strncpy(usuario_actual, nombre, sizeof(usuario_actual) - 1);
    usuario_actual[sizeof(usuario_actual) - 1] = '\0';
}

const char *ruta_usuario(const char *base) {
    snprintf(ruta, sizeof(ruta), "%s_%s", usuario_actual, base);
    return ruta;
}

int nombre_valido(const char *nombre) {
    if (nombre[0] == '\0') return 0;
    for (; *nombre; nombre++) {
        if (!isalnum((unsigned char)*nombre) && *nombre != '_') return 0;
    }
    return 1;
}

void renombrar_archivos_usuario(const char *viejo, const char *nuevo) {
    char origen[128], destino[128];
    for (size_t i = 0; i < NUM_ARCHIVOS; i++) {
        snprintf(origen, sizeof(origen), "%s_%s", viejo, ARCHIVOS[i]);
        snprintf(destino, sizeof(destino), "%s_%s", nuevo, ARCHIVOS[i]);
        rename(origen, destino); // si el archivo no existe, simplemente falla y no pasa nada
    }
}

void eliminar_archivos_usuario(const char *nombre) {
    char archivo[128];
    for (size_t i = 0; i < NUM_ARCHIVOS; i++) {
        snprintf(archivo, sizeof(archivo), "%s_%s", nombre, ARCHIVOS[i]);
        remove(archivo);
    }
}
