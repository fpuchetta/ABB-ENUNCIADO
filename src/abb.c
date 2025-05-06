#include "abb.h"
#include "abb_estructura_privada.h"

typedef struct abb_vec_t{
    void** vector;
    size_t capacidad;
    size_t guardados;
}abb_vec_t;

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
    nodo_nuevo->elemento=(void*)elemento;

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
 * Busca un elemento en el ABB.
 * 
 * Devuelve true si el elemento existe, false si no existe o en caso de error.
 */
bool abb_existe(const abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador) return false;
    if (!abb->raiz) return false;

    void* elemento_buscado=abb_buscar(abb,elemento);

    return (elemento_buscado) ? true : false;
}

nodo_t* abb_buscar_nodo_padre(const abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador) return NULL;
    if (!abb->raiz) return NULL;

    nodo_t* actual=abb->raiz;
    nodo_t* padre=NULL;
    bool encontrado=false;

    while(!encontrado && actual!=NULL){
        padre=actual;
        int cmp=abb->comparador(elemento,actual->elemento);
        if (cmp==0) encontrado=true;
        else if(cmp>0) actual=actual->der;
        else actual=actual->izq;
    }
    return (encontrado) ? padre : NULL;
}

nodo_t* abb_buscar_nodo(const abb_t *abb, const void *elemento){
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

    return actual;
}

/**
 * Busca un elemento en el ABB y lo devuelve.
 * 
 * Devuelve el elemento si existe, NULL si no existe o en caso de error.
 */
void *abb_buscar(const abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador) return NULL;
    if (!abb->raiz) return NULL;

    nodo_t* buscado=abb_buscar_nodo(abb,elemento);
    
    return (buscado) ? (void*)buscado->elemento : NULL;
}

void* sacar_nodo_hoja(abb_t* abb ,nodo_t* padre,void* elemento){
    if (!abb || !abb->comparador || !abb->raiz) return NULL;
    void* aux=NULL;
    nodo_t* buscado=NULL;

    if(!padre && abb->comparador(elemento,abb->raiz->elemento)==0){
        aux=abb->raiz->elemento;
        free(abb->raiz);
        abb->raiz=NULL;
        return aux;
    }

    if (abb->comparador(elemento,padre->elemento) > 0){
        buscado=padre->der;
        aux=buscado->elemento;
        free (buscado);
        padre->der=NULL;
    }else{
        buscado=padre->izq;
        aux=buscado->elemento;
        free(buscado);
        padre->izq=NULL;
    }
    
    abb->nodos-=1;
    return aux;
}

void* sacar_nodo_un_hijo(abb_t* abb,nodo_t* padre,void* elemento){
    if (!abb || !abb->comparador || !abb->raiz) return NULL;

    void* aux=NULL;
    nodo_t* buscado=NULL;

    if(!padre && abb->comparador(elemento,abb->raiz->elemento)==0){
        aux=abb->raiz->elemento;
        buscado=abb->raiz;
        (buscado->der) ? (abb->raiz=abb->raiz->der) : (abb->raiz=abb->raiz->izq);
        free(buscado);
        return aux;
    }

    if (abb->comparador(elemento,padre->elemento) > 0){
        buscado=padre->der;
        if(buscado->der)
            padre->der=buscado->der;
        else
            padre->der=buscado->izq;
    }else{
        buscado=padre->izq;
        if(buscado->der)
            padre->izq=buscado->der;
        else
            padre->izq=buscado->izq;

    }

    aux=buscado->elemento;
    free (buscado);
    abb->nodos-=1;
    return aux;
}

void* sacar_nodo_dos_hijos(abb_t* abb, nodo_t* buscado, void* elemento) {
    if (!abb || !buscado || !abb->comparador) return NULL;

    nodo_t* padre_predecesor = buscado;
    nodo_t* predecesor = buscado->izq;

    while (predecesor->der) {
        padre_predecesor = predecesor;
        predecesor = predecesor->der;
    }

    void* aux = buscado->elemento;
    buscado->elemento = predecesor->elemento;

    if (predecesor->izq == NULL)
        sacar_nodo_hoja(abb,padre_predecesor,elemento);
    else
        sacar_nodo_un_hijo(abb,(padre_predecesor == buscado) ? buscado : padre_predecesor, predecesor->elemento);

    return aux;
}

/**
 * Busca un elemento en el ABB y lo elimina.
 * 
 * Devuelve el elemento eliminado si existe, NULL si no existe o en caso de error.
 */
void *abb_sacar(abb_t *abb, const void *elemento){
    if (!abb || !abb->comparador || !abb->raiz) return NULL;

    nodo_t* padre=abb_buscar_nodo_padre(abb,elemento);
    nodo_t* buscado=NULL;
    if (!padre) return NULL;

    if (abb->comparador(elemento,padre->elemento) > 0)
        buscado=padre->der;
    else if (abb->comparador(elemento,padre->elemento)==0)
        buscado=padre;
    else
        buscado=padre->izq;
    
    if (buscado->der == NULL && buscado->izq == NULL)
        return sacar_nodo_hoja(abb,padre,(void*)elemento);
    else if ((buscado->izq == NULL && buscado->der != NULL) || (buscado->izq != NULL && buscado->der == NULL))
        return sacar_nodo_un_hijo(abb,padre,(void*)elemento);
    else
        return sacar_nodo_dos_hijos(abb,buscado,(void*)elemento);
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
    return (!abb || abb_tamanio(abb)==0) ? true : false;
}

size_t abb_recorrer_general(nodo_t* raiz,enum abb_recorrido modo,bool (*f)(void *, void *), void *ctx,bool* seguir_iterando){
    if (!raiz || !f || *seguir_iterando == false) return  0;


    size_t iterados=0;

    if (modo==ABB_PREORDEN){// act izq der
        iterados++;
        if (!f(raiz->elemento, ctx)) {
            *seguir_iterando = false;
            return iterados;
        }
    }

    iterados+=abb_recorrer_general(raiz->izq,modo,f,ctx,seguir_iterando);
    if (*seguir_iterando == false) return iterados;

    if (modo==ABB_INORDEN){ // izq act der
        iterados++;
        if (!f(raiz->elemento, ctx)) {
            *seguir_iterando = false;
            return iterados;
        }
    }

    iterados+=abb_recorrer_general(raiz->der,modo,f,ctx,seguir_iterando);
    if (*seguir_iterando == false) return iterados;

    if (modo==ABB_POSTORDEN){ // izq der act 
        iterados++;
        if (!f(raiz->elemento, ctx)) {
            *seguir_iterando = false;
            return iterados;
        }
    }

    return iterados;
}

/**
 * Recorre el ABB en el orden indicado y aplica la función f a cada elemento.
 * 
 * Devuelve la cantidad de elementos recorridos.
 */
size_t abb_recorrer(const abb_t *abb, enum abb_recorrido modo,bool (*f)(void *, void *), void *ctx){
        if (!abb || !abb->raiz || !f) return 0;

        bool iterar=true;
        size_t iterados=abb_recorrer_general(abb->raiz,modo,f,ctx,&iterar);
        return iterados;
}

bool insertar_vector(void* a_insertar, void* vector){
    if (!a_insertar || !vector) return false;

    abb_vec_t* vec= vector;

    if (vec->guardados >= vec->capacidad) return false;

    vec->vector[vec->guardados]= a_insertar;
    vec->guardados+=1;    
    return true;
}

/**
 * Recorre el ABB en el orden indicado y guarda los elementos en el vector, sin exceder la capacidad.
 * 
 * Devuelve la cantidad de elementos guardados.
 */
size_t abb_vectorizar(const abb_t *abb, enum abb_recorrido modo, void **vector,size_t capacidad){
    if (!abb || !vector || capacidad==0) return 0;

    abb_vec_t vector_abb = {.vector=vector, .capacidad=capacidad, .guardados=0};

    abb_recorrer(abb,modo,insertar_vector,&vector_abb);

    return vector_abb.guardados;
}

void destruir_recursivo(nodo_t* raiz,void (*f)(void*)){
    if (!raiz) return;

    destruir_recursivo(raiz->izq,f);
    destruir_recursivo(raiz->der,f);
    
    if (f != NULL){
        f(raiz->elemento);
    }
    free(raiz);
}

/**
 * Destruye el ABB y libera toda la memoria asignada.
 */
void abb_destruir(abb_t *abb){
    if (!abb || !abb->raiz) return;
    
    destruir_recursivo(abb->raiz,NULL);
    free(abb);
}

/**
 * Destruye el ABB y libera toda la memoria asignada, aplicando la función destructor a cada elemento.
 * 
 * Si el destructor es NULL, no se aplica ninguna función a los elementos.
 */
void abb_destruir_todo(abb_t *abb, void (*destructor)(void *)){
    if (!abb || !abb->raiz || !destructor) return;
    
    destruir_recursivo(abb->raiz,destructor);
    free(abb);
}