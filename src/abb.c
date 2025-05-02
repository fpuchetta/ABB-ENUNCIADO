#include "abb.h"
#include "abb_estructura_privada.h"

/**
 * Crea un ABB vacío con el comparador dado.
 * En caso de error devuelve NULL.
 */
abb_t *abb_crear(int (*cmp)(const void *, const void *)){
    if (!cmp) return NULL;

    abb_t* abb=calloc(1,sizeof(abb_t));
    if (!abb) return NULL;

    abb->comparador=cmp;
    return abb;
}


/**
 * Inserta un elemento en el ABB. Admite elementos repetidos.
 * 
 * Devuelve true si se pudo insertar el elemento, false si no pudo.
 */
bool abb_insertar(abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador) return false;

    nodo_t* nodo_nuevo=calloc(1,sizeof(nodo_t));
    if (!nodo_nuevo) return false;
    nodo_nuevo->elemento=elemento;

    if (!abb->raiz){
        abb->raiz=nodo_nuevo;
        abb->nodos+=1;
        return true;
    }

    nodo_t* nodo_actual=abb->raiz;
    nodo_t* nodo_padre=NULL;
    while(nodo_actual!=NULL){
        nodo_padre=nodo_actual;
        int comparacion=abb->comparador(elemento,nodo_actual->elemento);
        (comparacion<=0) ? (nodo_actual=nodo_actual->izq) : (nodo_actual=nodo_actual->der); 
        //los repetidos a la izq;
    }

    int comparacion=abb->comparador(elemento,nodo_padre->elemento);
    (comparacion<=0) ? (nodo_padre->izq=nodo_nuevo) : (nodo_padre->der=nodo_nuevo); 
    abb->nodos+=1;
    return true;
}

/**
 * Devuelve la cantidad de elementos en el ABB.
 */
size_t abb_tamanio(const abb_t *abb){
    return (!abb) ? 0 : (abb->nodos);
}

/**
 * Devuelve true si el ABB está vacío, false si no lo está.
 * 
 * Un árbol que no existe también se considera vacío.
 */
bool abb_vacio(const abb_t *abb){
    (!abb || abb_tamanio(abb)==0) ? true : false;
}

/**
 * Busca un elemento en el ABB y lo devuelve.
 * 
 * Devuelve el elemento si existe, NULL si no existe o en caso de error.
 */
void *abb_buscar(const abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador) return NULL;
    if (!abb->raiz) return NULL;

    bool encontrado=false;
    nodo_t* actual=abb->raiz;
    
    while (!encontrado && actual!=NULL){
        int cmp=abb->comparador(elemento,actual->elemento);
        if (cmp==0) encontrado=true;
        else if(cmp>0) actual=actual->der;
        else actual=actual->izq;
    }
    
    return (encontrado) ? (void*)actual->elemento : NULL;
}

/**
 * Destruye el ABB y libera toda la memoria asignada.
 */
void abb_destruir(abb_t *abb){
    //recorro todo el abb
    //destuyo nodo por nodo
}
