#include <iostream>
#include <time.h>
using namespace std;

int cardinalita = 9; //Variabile globale per tenere conto di quanti vertici mi restano
int numeroarchi = 21;

//CELLA ARCO PER LA LISTA DI ARCHI
struct arco{
	int nodo1, nodo2;
	arco* next;
};
typedef arco* edge_list;

//CELLA VERTICE PER LA LISTA DI VERTICI
struct vertice{
	int info;
	vertice* next;
};
typedef vertice* vertex_list;

//AGGIUNGO ARCO
edge_list addEdge(int nodo1, int nodo2, edge_list& lista){
	arco* temp = new arco;
	temp->nodo1 = nodo1;
	temp->nodo2 = nodo2;
	temp->next = NULL;
	if(lista == NULL){
		lista = temp;
		return lista;
		}
	arco* cur = lista;
	while(cur->next != NULL)
		cur = cur->next;
	cur->next = temp;
	return lista;
}

//AGGIUNGO VERTICE
vertex_list addVertex(int nodo, vertex_list& lista){
	vertice* temp = new vertice;
	temp->info = nodo;
	temp->next = NULL;
	if(lista == NULL){
		lista = temp;
		return lista;
		}
	vertice* cur = lista;
	while(cur->next != NULL)
		cur = cur->next;
	cur->next = temp;
	return lista;
}

//INIZIALIZZO GRAFO FRITSCH
edge_list FritschEdges(){
  arco* temp = NULL;
  temp = addEdge(1,9,temp);   temp = addEdge(2,8,temp);   temp = addEdge(4,7,temp);
  temp = addEdge(1,7,temp);   temp = addEdge(2,9,temp);   temp = addEdge(5,8,temp);
  temp = addEdge(1,2,temp);   temp = addEdge(6,5,temp);   temp = addEdge(9,7,temp);
  temp = addEdge(1,3,temp);   temp = addEdge(6,8,temp);   temp = addEdge(9,8,temp);
  temp = addEdge(1,4,temp);   temp = addEdge(6,9,temp);   temp = addEdge(4,5,temp);
  temp = addEdge(2,3,temp);   temp = addEdge(6,7,temp);   temp = addEdge(4,3,temp);
  temp = addEdge(2,5,temp);   temp = addEdge(6,4,temp);   temp = addEdge(3,5,temp);
  return temp;
}

//PRENDO UN VERTICE CASUALE TRA QUELLI DISPONIBILI
vertice* randomVertex(vertex_list list){
  int temp = rand()%cardinalita;
  vertice* cur = list;
  for(int i = 0; i < temp; i++)
    cur = cur->next;
  return cur;
}

//SVUOTO LA LISTA DI VERTICI(serve dopo aver eliminato un elemento)
vertex_list clearVertex(vertex_list lista){
	if(lista != NULL){
		lista->next = clearVertex(lista->next);
		delete lista;
		lista = NULL;
	}
	return lista;
}

//ELIMINAZIONE VETTORE(necessita di pulizia della vecchia lista che viene scartata)
vertex_list removeVertex(vertex_list lista, int nodo){
  vertex_list cur = lista;
  vertex_list prev = lista;
  vertex_list temp = NULL;
  while(cur != NULL){
    if(cur->info == nodo){
      prev = cur;
      cur = cur->next;
    }
    else{
      temp = addVertex(cur->info, temp);
      prev = cur;
      cur = cur->next;
    }
  }
	lista = clearVertex(lista); //Necessario perchè se no la memoria si riempe di spazzatura
  return temp;
}

//CAMBIO GLI ARCHI AVENTI NODI ELIMINATI(metto nuovo nodo di arrivo o di partenza)
edge_list changeEdge(edge_list& lista, int nodo, int newnode){
	edge_list cur = lista;
	while(cur != NULL){
		if(cur->nodo1 == nodo)
			cur->nodo1 = newnode;
		if(cur->nodo2 == nodo)
			cur->nodo2 = newnode;
		cur = cur->next;
		}
	return lista;
}

//PULISCO LISTA DI ARCHI(come per la lista dei vertici)
edge_list clearEdges(edge_list lista){
	if(lista != NULL){
		lista->next = clearEdges(lista->next);
		delete lista;
		lista = NULL;
	}
	return lista;
}

//ELIMINAZIONE ARCHI
edge_list removeEdge(edge_list lista, int nodo1, int nodo2){
  edge_list cur = lista;
  edge_list prev = lista;
  edge_list temp = NULL;
  while(cur != NULL){
    if(cur->nodo1 == nodo1 && cur->nodo2 == nodo2){
      prev = cur;
      cur = cur->next;
    }
    else{
      temp = addEdge(cur->nodo1, cur->nodo2, temp);
      prev = cur;
      cur = cur->next;
    }
  }
	lista = clearEdges(lista); //Necessario perchè se no la memoria si riempe di spazzatura
  return temp;
}

//CONTA ARCHI RIMASTI
int countEdge(edge_list lista){
	edge_list cur = lista;
	int counter = 0;
	while(cur != NULL){
		counter++;
		cur = cur->next;
		}
	return counter;
}

//I DUE VERTICI SELEZIONATI HANNO UN ARCO CHE LI UNISCE?
bool theresEdge(int nodo1, int nodo2, edge_list lista){
	edge_list cur = lista;
	while(cur != NULL){
		if((cur->nodo1 == nodo1 && cur->nodo2 == nodo2) || (cur->nodo1 == nodo2 && cur->nodo2 == nodo1))
			return true;
		cur = cur->next;
		}
	return false;
}

/*MCMin-Cut:
1. Seleziono due vertici a caso
2. Verifico che siano legati da un arco, se non lo sono rifaccio la scelta casuale
3. Se fossero legati da un arco vado avanti
4. Rimuovo il vertice indicato come montecarlo2->info
5. Rimuovo gli archi da montecarlo1->info a montecarlo2->info e viceversa,
   perchè il grafo non è orientato
6. Cambio gli archi che anno il nodo eliminato come nodo di partenza o di arrivo,
   mettendo come partenza o arrivo il vertice "sopravvissuto" al taglio che rappresenta
	 la fusione dei due nodi presi prima
7. Diminuisco la cardinalità, ciclo fino a cardinalita = 2*/
int MCMincut(vertex_list vertList, edge_list edgeList){
	while(cardinalita > 2){
		bool found = false;
		vertice* montecarlo2 = NULL;
		vertice* montecarlo1 = NULL;
		while(!found){
			montecarlo1 = randomVertex(vertList);
			montecarlo2 = randomVertex(vertList);
			found = theresEdge(montecarlo1->info, montecarlo2->info, edgeList);
		}
		//cout << "Taglio nodi tra: " << montecarlo1->info << " - " << montecarlo2->info << endl;
		vertList = removeVertex(vertList, montecarlo2->info);
		edgeList = removeEdge(edgeList, montecarlo1->info, montecarlo2->info);
		edgeList = removeEdge(edgeList, montecarlo2->info, montecarlo1->info);
		edgeList = changeEdge(edgeList, montecarlo2->info, montecarlo1->info);
		cardinalita--;
		}

	return countEdge(edgeList);
}

int main(){
	int p = 0;
	srand(time(NULL));
	for(int i = 0; i < 100000; i++){
		vertex_list vertList = NULL;
		for(int j = 0; j < cardinalita; j++)
			vertList = addVertex(j+1, vertList);

		edge_list edgeList = NULL;
		edgeList = FritschEdges();
		int mincut = MCMincut(vertList, edgeList);
		//cout << mincut << endl;
		cardinalita = 9;
		if(mincut == 4)
			p++;
		}
	cout << p << endl;
}
