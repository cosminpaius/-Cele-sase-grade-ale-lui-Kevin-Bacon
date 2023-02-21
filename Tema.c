#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define maxelem 7500

typedef struct node
{
    int val;
    struct node* next;
} node_t;

typedef struct Graph
{
    node_t* adjlist[maxelem];
    int visited[maxelem];
} graph ;

node_t* crearenod(int v)    //functie care returneaza un nod a carui valoare este data prin parametru 
{
    node_t* new = (node_t*)calloc(1, sizeof(node_t));
    new->val = v;
    new->next = NULL;
    return new;
}

void adaugmuchie(graph* graf, int nod1, int nod2) //adaug valoarea nod2 la lista de adiacenta a lui nod1
{
    node_t* new = crearenod(nod2);
    node_t* aux = graf->adjlist[nod1];
    if(aux == NULL)
        {
        graf->adjlist[nod1] = new;
        graf->adjlist[nod1]->next = NULL;
        return;
        }
    while(aux->next)
    {
        aux = aux->next;    //deplasare pana pe ultima pozitie
    }
    aux->next = new;
    aux->next->next = NULL;
}

char* numeactori[maxelem];    //aici se vor copia toate numele actorilor
int nr_nodes = 0;           // numarul total de actori
char act1[100], act2[100];  //vectori pentru citirea celor 2 actori de la task-ul 2

void citire(FILE *fp, graph* graf)
{
	int nrfilme, nract, i, j, k, nrfilmact = 0;
    int temp[maxelem];
	char numefilm[100], numeact[100], aux;
	fscanf(fp, "%d", &nrfilme); //citirea numarului de filme
    fgetc(fp);  //citire '\n'
	while(!feof(fp))
		{	nrfilmact ++;
            fgets(numefilm, 100, fp);   //citire nume film, inclusiv '\n'
			fscanf(fp, "%d", &nract);   //citirea numarului de actori din filmul respectiv
            fgetc(fp);  //citire '\n'
            if(feof(fp) == 1)   //in cazul in care fisierul s-a terminat
                break;
			for( i = 0 ; i <= nract - 1 ; i ++ )
				{	int gasit = 0;
					fgets(numeact, 100, fp);    //citire actor
                    for(j = 0 ; j < nr_nodes; j++)
                        if(strcmp(numeactori[j], numeact) == 0) //verific daca el a mai fost prezent in alt film
                            {
                                temp[i] = j;    //copiez pozitia initiala din filmul in care a mai fost prezent
                                gasit = 1;
                                break;
                            }
                    if(gasit == 0)  //daca nu a fost gasit anterior
                            {
                            numeactori[nr_nodes] = (char*)calloc(100, sizeof(char));
                            strcpy(numeactori[nr_nodes], numeact);  
                            temp[i] = nr_nodes; //copiez numarul total de actori pana la acest moment
                            nr_nodes ++;
                            }
                    for(k = 0 ; k < i ; k++)    //creez legaturi intre actorul curent si actorii cititi pana acum
                        {                       //din filmul curent
                        adaugmuchie(graf, temp[i], temp[k]);
                        adaugmuchie(graf, temp[k], temp[i]);  
                        }
                }
			if(nrfilmact == nrfilme)
				break;
        }
	if(feof(fp) == 0)   //citirea celor doi actori pentru task-ul al 2-lea
		{
			fgets(act1, 100, fp);
			fgets(act2, 100, fp);
		}
}

void DFS(graph* graf, int nod, int* nrnod)  //DFS cu numarare de noduri parcurse pentru a afla componenta maxima
{
    node_t* temp = graf->adjlist[nod];
    int aux;
    graf->visited[nod] = 1;
    (*nrnod) ++;
    while (temp != NULL) 
    {   
        aux = temp->val;
        if (graf->visited[aux] == 0) 
            DFS(graf, aux, nrnod);
        temp = temp->next;
    }
}

int connected_components(graph* graf, int *max) //functie prin care parcurg nodurile si retin nr de elem din
                                                //componenta maxima si pozitia unui actor ce apartine acesteia
{
    int nr = 0, i, pozitie;
    for(i = 0 ; i < nr_nodes; i++)  //nr_nodes = nr. de noduri
        if(graf->visited[i] == 0)
            {   nr = 0;
                DFS(graf, i, &nr);
                if(nr > (*max)) //verific daca nr de noduri actual este mai mare decat maximul calculat anterior
                    {
                        (*max) = nr;
                        pozitie = i;    //retin pozitia unuia dintre actorii celei mai mari comp. conexe
                    }
            }
    return pozitie;
}

char* sortnames[maxelem];   //vector pentru retinerea numelor din componenta conexa maxima
int nrnume; 

void createsortnames(graph* graf, int poz)  //parcurg lista de adiacenta a unui nod dat, afland numarul de nume
                                            //si copiindu-le pe acestea intr-un vector separat
{
    node_t* temp = graf->adjlist[poz];
    int aux;
    graf->visited[poz] = 1;
    sortnames[nrnume] = (char*)calloc(100, sizeof(char));
    strcpy(sortnames[nrnume], numeactori[poz]);
    nrnume ++;
    while (temp != NULL) 
    {   
        aux = temp->val;
        if (graf->visited[aux] == 0) 
            createsortnames(graf, aux);
        temp = temp->next;
    }
}

void sortname() //functie de sortare a vectorului global ce contine numele din componenta maxima
{
    int i, j;
    for(i = 0 ; i < nrnume - 1; i++)
        for(j = i + 1 ; j < nrnume ; j++)
            if(strcmp(sortnames[i], sortnames[j]) > 0)
                {
                    char vector[100];
                    strcpy(vector, sortnames[i]);
                    strcpy(sortnames[i], sortnames[j]);
                    strcpy(sortnames[j], vector);
                }
}

void eliberare1(node_t* head)
{
    if(head->next)
        eliberare1(head->next);
    free(head);
}

void eliberare(graph* graf)
{   
    int i;
    for(i = 0 ; i < nr_nodes; i++)
        if(graf->adjlist[i] != NULL)
            eliberare1(graf->adjlist[i]);
}

void print(FILE* fo)
{   int i;
    for( i = 0 ; i < nrnume ; i++)
        fprintf(fo, "%s", sortnames[i]);
}

void push_back ( node_t** deq , int value ) //fucntie prin care adaug la sfarsitul unei liste o valoare
{
    node_t *new, *aux = *deq;
    new = (node_t*)calloc(1, sizeof(node_t));
    new->val = value; 
    if(aux != NULL)
        {   
            while(aux->next)
                aux = aux->next;
            aux->next = new;
            new->next = NULL;
        }  
    else
        {
            *deq = new;
            (*deq)->next = NULL;
        } 
}

void pop_front (node_t* *deq)   //functie prin care elimin de la inceputul listei un element
{
    node_t *temp = (*deq);
	if((*deq) == NULL)
		return;
    if((*deq) != NULL && (*deq)->next == NULL)  //verific daca exista un singur element in lista
        {
			free((*deq));
			(*deq) = NULL;
            return;
        }
    if((*deq)->next != NULL)
        {
        (*deq) = (*deq)->next;
        free(temp);
        }
}

void graph_traverse_BFS(FILE* fo, graph* graf, int start_node_id, int finish_node_id, int distanta[maxelem])
{   
    int i, temp;
    node_t* list = NULL;
    node_t* aux;
    for(i = 0; i < nr_nodes ; i++)
    {
        distanta[i] = INT_MAX;  //initializarea tuturor distantelor cu INT_MAX
    }
    distanta[start_node_id] = 0;    //distanta de la nodul de plecare pana la el este 0
    push_back(&list, start_node_id);   //il adaug in lista
    while( list != NULL )
    {
        temp = list->val;   // copiez valoarea din capul listei
        pop_front(&list);   // elimin nodul din capul listei
        aux = graf->adjlist[temp];  // retin primul element din lista de adiacenta 
        while(aux)  //parcurg lista de adiacenta
        {	
            if( distanta[aux->val] == INT_MAX && aux->val != start_node_id) //daca distanta nodului nu a fost modificata
            {
                distanta[aux->val] = distanta[temp] + 1;    //primeste valoarea distantei nodului "parinte" + 1
                push_back(&list, aux->val); //adaug nodul in lista
            }
			if(aux->val == finish_node_id)  //verific daca am ajuns la nodul pentru care calculez distanta
                break;
            aux = aux->next;
        }
    }
	if(list)
		eliberare1(list);
	free(list);
    if(distanta[finish_node_id] == INT_MAX) //daca distanta nu a fost modificata, nodurile nu fac parte din aceeasi 
        distanta[finish_node_id] = -1;  //componenta conexa
}

char* actc3[maxelem];
int nrpunti;

void DFSB(graph* G, int v, int timp, int* idx, int* low, int* pi)
{   
    int u;
    idx[v] = timp;  //timpul de descoperire al nodului
    low[v] = timp;  //retine timpul minim
    timp = timp + 1;
    node_t* aux = G->adjlist[v];    //pentru parcurgerea listei de adiacenta
    while(aux)
    {   
        u = aux->val;   
        if(u != pi[v])
            if(idx[u] == -1)    //nodul este nedescoperit
            {
                pi[u] = v;
                DFSB(G, u, timp, idx, low, pi);
                if(low[u] < low[v])
                    low[v] = low[u];
                if(low[u] > idx[v]) //a fost gasita puntea
                {
                    char str[202];
                    int i;
                    if(strcmp(numeactori[u], numeactori[v]) < 0)
                        {   
                            strcpy(str, numeactori[u]);
                            str[strlen(numeactori[u]) - 1] = ' ';
                            strcat(str, numeactori[v]);
                        }
                    else
                        {
                            strcpy(str, numeactori[v]);
                            str[strlen(numeactori[v]) - 1] = ' ';
                            strcat(str, numeactori[u]);
                        }
                    actc3[nrpunti] = (char*)calloc(202, sizeof(char));
                    strcpy(actc3[nrpunti], str);    //adaug numele actorilor ce creeaza puntea
                                                    //la vectorul de nume 
                    nrpunti ++;
                }    
            }
            else
                if(idx[u] < low[v])     //nodul u este descoperit
                    low[v] = idx[u];
        aux = aux->next; 
    }
}

void punti(graph* G)
{   
    int v, timp, idx[maxelem], pi[maxelem], low[maxelem];
    //idx retine timpul de descoperire al nodului
    //low retine timpul minim
    //pi retine parintele nodului
    timp = 0;
    for(v = 0 ; v < nr_nodes ; v++)
        {
            idx[v] = -1;    //timpul de descoperire al nodului
            low[v] = INT_MAX;
            pi[v]  = 0;
        }
    for(v = 0 ; v < nr_nodes ; v++)
        if(idx[v] == -1)    //nodul este nedescoperit
            DFSB(G, v, timp, idx, low, pi);
}

void sortc3()   //functie de sortare a numelor de la cerinta 3
{
    int i, j;
    char temp[202];
    for(i = 0 ; i < nrpunti - 1 ; i++)
        for(j = i + 1; j < nrpunti ; j++)
            if(strcmp(actc3[i], actc3[j]) > 0)
                {
                    strcpy(temp, actc3[i]);
                    strcpy(actc3[i], actc3[j]);
                    strcpy(actc3[j], temp);
                }
}
int main(int argc, char* argv[])
{
    int i, nrnods = 0, pozitie;
	graph* graf;
	graf = (graph*)calloc(1, sizeof(graph));
    FILE* fp = fopen(argv[2], "r");
    FILE* fo = fopen(argv[3], "w");
    citire(fp, graf);
    for(i = 0 ; i < nr_nodes; i++)
        {
        graf->visited[i] = 0;
        } 
	if(strcmp(argv[1], "-c1") == 0)
    {
    	pozitie = connected_components(graf, &nrnods);
    	for(i = 0 ; i < nr_nodes; i++)
        	{
        	graf->visited[i] = 0;
        	} 
    	createsortnames(graf, pozitie);
    	sortname();
    	fprintf(fo, "%d\n", nrnods);
    	print(fo);
    }
	if(strcmp(argv[1], "-c2") == 0)
	{
		int i, nrnods = 0, pozitie, poz1, poz2, dist[maxelem];
    	for(i = 0 ; i < nr_nodes ; i++)
			{
			if(strcmp(numeactori[i], act1) == 0)
				poz1 = i;
			if(strcmp(numeactori[i], act2) == 0)
				poz2 = i;
			}
    	graph_traverse_BFS(fo, graf, poz1, poz2, dist);
		fprintf(fo, "%d\n", dist[poz2]);
	}
	if(strcmp(argv[1], "-c3") == 0)
	{
		punti(graf);
        sortc3();
        fprintf(fo, "%d\n", nrpunti);
        for(i = 0 ; i < nrpunti ; i ++)
            fprintf(fo, "%s", actc3[i]);
	}
	for(i = 0 ; i < nr_nodes ; i++)
		free(numeactori[i]);
    for(i = 0 ; i < nrpunti ; i++)
		free(actc3[i]);
    for(i = 0 ; i < nrnume ; i++)
        free(sortnames[i]);
	eliberare(graf);
    free(graf);
    fclose(fp);
    fclose(fo);
}