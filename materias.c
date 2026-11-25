// Módulo: Materias y calificaciones

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "materias.h"
#include "usuario.h"

#define ARCHIVO_MATERIAS ruta_usuario("materias.dat")
//Su arreglo dinamico (privado)

static Materia *materias = NULL;
static int total = 0;   //materias guardadas
static int capacidad = 0; //sus espacios reservados 
//Estamos usando static para la privacidad del codigo, esta variable es de uso de solamente este archivo, evitando un error de duplicado

//Utilidades de lectura
static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Int c nos va a ayudar a recibir el caracter, while nos va a ayudar a leer y descartar lo que queda escrito
//y su salto en linea junto con el EOF son quienes detienen el programa, dejando el búfer en blanco 

static int leer_int(const char *msg, int min, int max){
    int v;
    while (1){
        printf("%s", msg);
        if (scanf("%d", &v) == 1 && v >= min && v <= max) { 
            limpiar_buffer();
            return v;
        }
        limpiar_buffer();
        printf("Valor invalido (%d a %d).\n", min, max);
    }
}
//Usamos while para que el bucle sea infinito y al momento de de que el programa tope el return v; lo obligue a salirse
//Si nos equivocamos o escribimos un numero fuera de rango, el ciclo no termina, hasta que le atinemos
//se intenta leer el número entero y se guarda en v

static float leer_float(const char *msg, float min, float max) {
    float v;
    while (1)
    {
        printf("%s", msg);
        if (scanf("%f", &v) == 1 && v >= min && v <= max) {
            limpiar_buffer();
            return v;
        }
        limpiar_buffer();
        printf("Valor invalido (%.0f a %.0f).\n", min, max);
    }
}
//Hace lo mismo que el anterior while, solamente que aqui usamos float(decimales)

static int parciales_completos(const Materia *m) {
    for (int i = 0; i < m ->num_parciales; i++)
    if (m->calificaciones [i] < 0) return 0;
    return 1;
}
//El puntero de *m (materias) esta apuntando directamente al arreglo de arriba, con la flecha se accede a los datos de los num_paciales
//con ayuda de for nos va a dar las vueltas de acuerdo a los parciales que haya, en cada vuelta se pide el porcentaje y se guarda en el arreglo de ponderaciones
//si el ciclo se encuentra con el -1, sig que falta esa nota por lo cual devuelve el 0 y cuando ya ninguno es menor que 0, se devuelve un 1 (que ya estan completos)

static int elegir_materia(void){
    if (total == 0){
    printf("Aún no hay materias.\n");
    return -1;
    }
    mostrar_materias();
    return leer_int("Número de materia: ", 1, total) - 1;
}
//Al elegir la materian solamente aparece una vez, si el total es igual a 0, se marca que no hay materia, regresando al usuario que escriba la materia
//finalmente, al escribir el numero de materia, se muestra

//Funciones del modulo 
void agregar_materias(void){
//Si el arreglo esta lleno, este se duplica con realloc
    if (total == capacidad) {
        int nueva = (capacidad == 0) ? 2 : capacidad * 2;
        Materia *tmp = realloc(materias, nueva * sizeof(Materia));
        if (tmp == NULL) {
            printf("No hay memoria disponible.\n");
            return;
        }
        materias = tmp;
        capacidad = nueva;

}
// Total se compara al num de materias que se guardo en total en el espacio de capacidad, llenando los huecos
//y cuando se llena, if agranda el espacio. Al no haber escrito nada, la memoria separa 2 espacios (capacidad)

Materia *m = &materias[total];

printf("Nombre de la materia: ");
fgets(m->nombre, MAX_NOMBRE, stdin);
m->nombre[strcspn(m->nombre, "\n")] = '\0';
//Se imprime el texto (nombre de la materia) que el usuario escribio, fgets ayuda a escribir con esapacios
//m->nombre es en donde se guarda el texto y MAX_NOMBRE es el limite permitidoy con stdin es donde el texto se va a cap en la entrada

    m->num_parciales = leer_int("Numero de parciales (1-7): ", 1, MAX_PARCIALES);
//m->num_parciales es en donde los números de parciales (1-7) y se valida, 1 y max_parciales son los limites permitidos, obligando al usuario a escribir el numero, asi?


float suma;
do {
    suma = 0;
    for (int i = 0; i < m->num_parciales; i++)
    {
        printf("Parcial %d - ", i + 1);
        m->ponderaciones[i] = leer_float("% de ponderacion: ", 0, 100);
        m->calificaciones[i] = -1;
        suma += m->ponderaciones[i];
    }
    //i al ser igual a 0 debe ser i menos que los parciales y este se suma ante cada pacial puesto
//m->ponderaciones (porcentaje) pasa de 0 al 100? y si no tiene algo registrado, se pasa como -1

    m->pond_final = leer_float("%  del examen final: ", 0, 100);
    suma += m->pond_final;
    if (fabs(suma - 100) > 0.01)
    printf("Las ponderaciones suman %.2f y deben sumar 100. Captura de nuevo.\n", suma);
} while (fabs(suma - 100) > 0.01);
    
total++;
printf("Materia agregada.\n");
}
//m->pond_final muestra el porcentaje del examen final, sumando poco a poco, haciendo tambien una comprobaciónn de margen de error con los num decimales, 
//cuando se logra acompletar, solamente se agrega

void capturar_calificacion(int indice){
    Materia *m = &materias[indice];
    int p = leer_int("Número de parcial: ", 1, m->num_parciales) -1;
    m->calificaciones[p] = leer_float("Calificacion (0-100): ", 0, 100);
    printf("Calificaciones guardadas.\n");
}

void mostrar_materias(void) {
    if (total == 0){
        printf("Aun no hay materias.\n");
        return;
    }
    for (int i = 0; i < total; i++)
    {
        Materia *m = &materias[i];
        printf("\n%d) %s\n", i + 1, m->nombre);
        for (int j = 0; j < m->num_parciales; j++){
            if (m->calificaciones[j] < 0)
                printf(" Parcial %d (%.0f%%): sin calificar \n", j + 1, m->ponderaciones[j]);
            else
                printf(" Parcial %d (%.0f%%): %.1f\n", j + 1, m->ponderaciones[j], m->calificaciones[j]);
        
        }
        printf(" Examen final: %.0f%%\n", m->pond_final);
    }   
}
//el if nos va a ayudar que la funcion revise si hay materias guardadas, si el total es 0, entra y se imprime el mensaje para detener el proceso. 
//El proceso for nos ayudara a que sera un indice de control que empieza de 0, 
//recorriendo las materias hasta llegar a su total y avanza de uno en uno. El filtro nos ayuda a que el programa revisa la calif del parcial que sea menor a cero
// el -1 es para los que no tienen calif y al tener si una calif, se pasa el else e imprime la calif

//Puntos ya ganados: suma de calificación * ponderacion / 100
float calcular_acumulado(const Materia *m) {
    float acum = 0;
    for (int i = 0; i < m->num_parciales; i++){
        if(m->calificaciones[i] >= 0)
            acum += m->calificaciones[i] * m->ponderaciones[i] / 100.0f;
    }
    return acum;
}
//Se calcula la calificacion de los parciales, usando la constante de materia,
//se usar for  para hacer el ciclo haceindo que el acumulado debe ser igual a 0, i debe ser meno r a los numeros parciales para despues incrementarse cuando el espacio e ocupado. el if nos ayuda a a que si las calificaciones son 
//mayores o igual a 0,  este se suma e iguala a las calificacioenes para multiplicar y dividir la calificacion, regresando al acumulado

//Calificación que se ocupa para el final. Devuelve -1 si la materia no tiene examen final

float calificacion_necesaria(const Materia *m, float meta) {
    if (m->pond_final <= 0) return -1; //-1 = no hay final
    float nec = (meta - calcular_acumulado(m)) / (m->pond_final / 100.0f);
    return (nec < 0) ? 0 : nec; //ya lo tenemos asegurado, no necesitamos sacar nada en el final
}
//Usaremos el float donde la calif necesaria sea con la constancia de la materia para  llefar a la meta, ir hace que el final sea menor o igual a 0, 
//siendo un filtro y si no hay calificacion, este te regresara el -1, 
//cuando se pase el filtro,se hace la matematica en donde se resta la meta con el lo acumulado, dividiendo este el final con el 100.0f,dando el num cerrado

static void consultar_necesaria(int indice){
    Materia *m = &materias[indice];
    
    if(!parciales_completos(m)) {
        printf("Ponga primero las calificaciones de todos los parciales.\n");
        return;
    }
    float meta = leer_float("Calificación final que deseas (ej. +70): ", 0, 100);
    float nec = calificacion_necesaria(m, meta);

    printf("\n Estado del acumulado actual: %.2f\n", calcular_acumulado(m));
    if (nec < 0)
        printf("Esta materia no tiene examen global.\n");
    else if (nec > 100)
        printf("No es posible: necesitarías %.1f.\n", nec);
    else if (nec < 0.0001f)    
        printf("Ya tienes asegurada esa calificación.\n");
    else
        printf("Necesitas sacar %.1f en el examen final para llegar a %.0f.\n", nec, meta);   
}
//Aqui al consultar la calif necesaria, se ponen las calif de los parciales, su meta que pasa del 0 al 100, su acumulado que solamente se ocupan do cifras, 
//si es menor a 0, imprime el primer printf, despues si es mayor a 100 muestra lo que necesias o si no es posible llegar a tal puntacion antes elegida, si 
//es igual a 0 o menor a lo que buscamos, esta o ya lo tienes asegurado o con el "Necesitas sacar..."  

//Promedio de una materia usando solo los parciales que ya tienen calificacion (sobre 100)
//Devuelve -1 si la materia aun no tiene ninguna calificacion
float promedio_materia(const Materia *m) {
    float pts = 0, pond = 0;
    for (int i = 0; i < m->num_parciales; i++) {
        if (m->calificaciones[i] >= 0) {
            pts += m->calificaciones[i] * m->ponderaciones[i];
            pond += m->ponderaciones[i];
        }
    }
    return (pond > 0) ? pts / pond : -1;
}
//Cada calificacion se multiplica por su ponderacion y se divide entre la suma de las ponderaciones ya calificadas,
//asi el promedio no se castiga por los parciales que todavia no se han capturado

//Promedio de todas las materias que ya tienen alguna calificacion. Devuelve -1 si ninguna tiene
float promedio_general(void) {
    float suma = 0;
    int n = 0;
    for (int i = 0; i < total; i++) {
        float p = promedio_materia(&materias[i]);
        if (p >= 0) {
            suma += p;
            n++;
        }
    }
    return (n > 0) ? suma / n : -1;
}
//Recorre todas las materias, suma los promedios que si existen y los divide entre cuantas materias se contaron

static void ver_promedios(void) {
    if (total == 0) {
        printf("Aun no hay materias.\n");
        return;
    }
    printf("\n PROMEDIOS \n");
    for (int i = 0; i < total; i++) {
        float p = promedio_materia(&materias[i]);
        if (p < 0)
            printf("%d) %s: sin calificaciones\n", i + 1, materias[i].nombre);
        else
            printf("%d) %s: promedio %.1f | acumulado %.2f pts\n",
                i + 1, materias[i].nombre, p, calcular_acumulado(&materias[i]));
    }
    float g = promedio_general();
    if (g >= 0)
        printf("\nPromedio general: %.1f\n", g);
}
//Muestra el promedio y los puntos acumulados de cada materia, y al final el promedio general

//Interfaz del resto

int total_materias(void) { return total; }

const char *obtener_nombre_materia(int i) {
    if (i < 0 || i >= total) return "";
    return materias[i].nombre;
}
//el total de materias decuelve cuantas materias hay guardadas
//if es el filtro para validar que el indice este en el rango correcto, regresando el nombre si es valido

void guardar_materias(void) {
    FILE *f = fopen(ARCHIVO_MATERIAS, "wb");
    if (f == NULL){
        printf("No se pudo guardar materias.\n");
        return;
    }
    fwrite(&total, sizeof(int), 1, f);
    fwrite(materias, sizeof(Materia), total, f);
    fclose(f);
}
//Se muestra solamente una vez gracias al void, creando el archivo de mas materias, 
//si f es nulo imprime que no hay materias, segresando al usuario a escribir las materias, 
//imprimiendo aquellas materias en total y confirmando con un 1, cerrando despues el archivo

void cargar_materias(void) {
    liberar_materias(); //evita duplicar datos o fugas si se carga mas de una vez (ej. al cambiar de sesion)
    FILE *f = fopen(ARCHIVO_MATERIAS, "rb");
    if (f == NULL) return; //primera vez: No hay archivo
    int n;
    if (fread(&n, sizeof(int), 1, f) != 1 || n < 0) {
        fclose(f);
        return;
    }
    capacidad = (n < 2) ? 2 : n;
    materias = malloc(capacidad * sizeof(Materia));
    if (materias == NULL){
        capacidad = 0;
        fclose(f);
        return;
        }
        total = (int)fread(materias, sizeof(Materia), n, f);
        fclose(f);
}
//Se abre el archivo binario en modo lectura. Si no existe, sig que es la primera vez y se detiene
//Si existe, lee primero cuantas materias hay guardadas (n), reserva la memoria con malloc,
//carga todas las materias de un solo golpe con fread y se cierra el archivo

void liberar_materias(void){
    free(materias);
    materias = NULL;
    total = 0;
    capacidad = 0;
}
//Libera la memoria dinamica reservada para el arreglo de materias, usando free y restablece las variables del control
//evitando fugas de memoria y dejar el módulo limpio al salir 

void menu_materias(void){
    int op;
    do
    {
        printf("\n MATERIAS Y CALIFICACIONES \n");
        printf("1. Agregar materia\n");
        printf("2. Ver materias\n");
        printf("3. Capturar calificacion de un parcial\n");
        printf("4. ¿Qué necesito en el examen final?\n");
        printf("5. Ver promedios\n");
        printf("0. Volver\n");
        op = leer_int("Opcion: ", 0, 5);

        switch (op)
        {
        case 1: agregar_materias(); guardar_materias(); break;
        case 2: mostrar_materias(); break;
        case 3: {int i = elegir_materia(); if (i >= 0) { capturar_calificacion(i); guardar_materias(); } break; }
        case 4: {int i = elegir_materia(); if (i >= 0) consultar_necesaria(i); break; }
        case 5: ver_promedios(); break;

        }
    } while (op != 0);
    
}
//El menu esta compuesto por casos, agregando materias y donde i debia de elergirlas para espues usar el if como filtro para capturar las calificaciones y/o su calculo necesario, regresando el op igual a 0
