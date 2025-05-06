#include "abb.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Definición del struct
typedef struct poke {
    const char* nombre;
    int id;
    int inteligencia;
} poke_t;

// Función para mostrar un Pokémon
void mostrar_pokemon(const poke_t* p) {
    if (!p) return;
    printf("Nombre: %s | ID: %d | Inteligencia: %d\n", p->nombre, p->id, p->inteligencia);
}

// Comparador por nombre
int comparar_pokes_nombre(const void* a, const void* b) {
    const poke_t* p1 = a;
    const poke_t* p2 = b;
    return strcmp(p1->nombre, p2->nombre);
}

bool mostrar_wrapper(void* elemento, void* ctx) {
    (void)ctx; // no usamos el contexto en este caso
    mostrar_pokemon((poke_t*)elemento);
    return true;  // para seguir recorriendo
}

bool mostrar_hasta_charizard(void* elemento, void* ctx) {
    poke_t* poke = (poke_t*)elemento;
    if (!poke) return true;

    mostrar_pokemon(poke);

    // Condición de corte
    if (strcmp(poke->nombre, "Charizard") == 0) {
        return false;  // Detener el recorrido
    }

    return true;  // Seguir recorriendo
}


int main() {
    abb_t* abb = abb_crear(comparar_pokes_nombre);  // asumimos que existe esta función

    struct poke p1 = { .nombre = "Pikachu",   .id = 25,  .inteligencia = 0 };
    struct poke p2 = { .nombre = "Lechonk",   .id = 915, .inteligencia = 10 };
    struct poke p3 = { .nombre = "Larvitar",  .id = 200, .inteligencia = 6 };
    struct poke p4 = { .nombre = "Charizard", .id = 6,   .inteligencia = 10 };
    struct poke p5 = { .nombre = "Raichu",    .id = 26,  .inteligencia = 1 };

    abb_insertar(abb, &p1);
    abb_insertar(abb, &p4);
    abb_insertar(abb, &p3);
    abb_insertar(abb, &p2);
    abb_insertar(abb, &p5);

    void *vector[5];  // Queremos almacenar solo 5 elementos

    size_t cantidad = abb_vectorizar(abb,ABB_POSTORDEN, vector, 3);

    printf("Cantidad guardada: %zu\n", cantidad);

    for (size_t i = 0; i < cantidad; i++) {
        struct poke* poke = (struct poke*)vector[i];
        mostrar_pokemon(poke);
    }
    
    printf("\nRecorrido hasta encontrar a Charizard (PREORDEN):\n");
    size_t iterados=abb_recorrer(abb, ABB_PREORDEN, mostrar_hasta_charizard, NULL);
    printf("Cantidad de iterados:%zu\n",iterados);
    
    abb_destruir(abb);

    return 0;
}
