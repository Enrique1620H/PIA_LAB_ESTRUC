#ifndef USUARIO_H
#define USUARIO_H

// Guarda el usuario activo; los modulos lo usan para armar el nombre de sus archivos
void establecer_usuario(const char *nombre);

// Devuelve "<usuario>_<base>", ej. ruta_usuario("gastos.dat") -> "Ana_gastos.dat"
// (usa un buffer interno: usar el resultado enseguida, no guardarlo)
const char *ruta_usuario(const char *base);

// Solo letras, numeros y guion bajo (evita nombres que rompan el nombre de archivo)
int nombre_valido(const char *nombre);

// Para Cambiar_Nombre y Eliminar_Cuenta
void renombrar_archivos_usuario(const char *viejo, const char *nuevo);
void eliminar_archivos_usuario(const char *nombre);

#endif
