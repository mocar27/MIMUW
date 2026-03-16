#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct node
{
    size_t value;
    struct node *next;
};
typedef struct node newNode;
struct queue
{
    newNode *first, *last;
};
typedef struct queue Tqueue;

// Queue functions
Tqueue *createQueue()
{
    Tqueue *q = malloc(sizeof(Tqueue));
    q->first = NULL;
    q->last = NULL;
    return q;
}
int empty(Tqueue *q)
{
    if (q->first == NULL)
        return 1;
    else
        return 0;
}
void push(Tqueue *q, size_t x)
{
    newNode *temp = calloc(1, sizeof(newNode));
    temp->value = x;
    temp->next = NULL;
    if (q->first == NULL)
    {
        q->first = temp;
        q->last = temp;
    }
    else
    {
        q->last->next = temp;
        q->last = temp;
    }
}
size_t pop(Tqueue *q)
{
    newNode *pom = q->first;
    size_t v = q->first->value;
    if (q->first == q->last)
        q->first = NULL;
    else
        q->first = q->first->next;
    if (q->first == NULL)
        q->last = NULL;
    free(pom);
    return v;
}

// Function creates a new node in a list with the value: var
newNode *createNode(size_t var)
{
    newNode *Vert = calloc(1, sizeof(newNode));
    Vert->value = var;
    Vert->next = NULL;
    return Vert;
}

// Graph functions: adding edges between nodes and adding a new node
void addEdge(newNode ***adjList, size_t a, size_t b)
{
    newNode *neighbour = createNode(b);
    newNode *temporary = (*adjList)[a];
    while (temporary->next != NULL)
        temporary = temporary->next;
    temporary->next = neighbour;
}
void addVertex(char ***vertex, newNode ***adjList, size_t *numOfVertices, char **string, size_t *length, size_t *beginning, int *exists)
{
    (*vertex)[*numOfVertices] = calloc(*length, sizeof(char));
    strcpy((*vertex)[*numOfVertices], *string);
    newNode *vert = createNode(*numOfVertices);
    (*adjList)[*numOfVertices] = vert;
    if (strcmp((*vertex)[*numOfVertices], "start") == 0)
    {
        *beginning = *numOfVertices;
        *exists = 1;
    }
    (*numOfVertices)++;
}

// Function finds positions "v1" and "v2" in the node name array
void find(char ***vertex, size_t *numOfVertices, char **v1, char **v2, size_t *a, size_t *b)
{
    for (size_t i = 0; i < *numOfVertices; i++)
    {
        if (strcmp(*v1, (*vertex)[i]) == 0)
            *a = i;
        if (strcmp(*v2, (*vertex)[i]) == 0)
            *b = i;
    }
}

// Function copies the string from the "string" array to the "v" array
void copy(char **v, char **string, int *counter, size_t *length)
{
    *v = calloc(*length, sizeof(char));
    strcpy(*v, *string);
    (*counter)++;
}

// Function reads the name of a node from the entry to the "string" array
void read(char **string, size_t *length, char *c)
{
    size_t it = 0;
    size_t temp = 100; // Variable needed to possibly expand the memory needed
    (*string)[it] = *c;
    it++;

    // We write the name of the node from the input in the loop and save it in the array "string"
    while (*c != '-' && *c != ' ' && *c != '\n' && *c != '}')
    {
        *c = (char)getchar();
        if (*c != '-' && *c != ' ' && *c != '\n' && *c != '}')
        {
            if (*length == temp)
            {
                temp *= 2;
                *string = realloc(*string, temp * sizeof(char));
            }
            (*string)[it] = *c;
            it++;
            (*length)++;
        }
    }
    (*length)++;
    *string = realloc(*string, (*length) * sizeof(char));
    (*string)[(*length) - 1] = '\0';
}

// Traversing the graph - breadth-first search
int BFS(size_t begin, size_t number, newNode ***adjList)
{
    int *visited = calloc(number, sizeof(int));
    int result = 0;
    Tqueue *Q = createQueue();
    push(Q, begin);
    while (empty(Q) == 0)
    {
        begin = pop(Q);
        if (visited[begin] == 0)
        {
            visited[begin] = 1;
            result++;
            while ((*adjList)[begin] != NULL)
            {
                push(Q, (*adjList)[begin]->value);
                newNode *delete = (*adjList)[begin];
                (*adjList)[begin] = (*adjList)[begin]->next;
                free(delete);
            }
        }
    }
    for (size_t x = 0; x < number; x++)
    {
        while ((*adjList)[x] != NULL)
        {
            newNode *del = (*adjList)[x];
            (*adjList)[x] = (*adjList)[x]->next;
            free(del);
        }
    }
    free(visited);
    free(Q);
    return result;
}

int main()
{
    int left = 0;             // Variable specifies that the graph representation has started
    int repetition = 0;       // Variable determines whether the node with the given name has occured before
    int counter = 0;          // Variable counting how many vertices we have encountered, to connect them later into neighbors
    char c = 0;               // Variable reading data from input
    int exists = 0;           // Variable that checks whether "start" node exists
    size_t beginning = 0;     // Variable holding in memory the positions of the node named "start"
    size_t numOfVertices = 0; // Variable that counts how many different vertices are in the graph
    size_t memory = 100;      // Variable needed to possibly expand the memory needed
    size_t a = 0;             // Variable holding the positions of word "v1" in the vertex array
    size_t b = 0;             // Variable holding the positions of word "v2" in the vertex array
    char *v1 = 0;
    char *v2 = 0;
    char **vertex = calloc(memory, sizeof(char *));        // An array with unique names of graph nodes
    newNode **adjList = calloc(memory, sizeof(newNode *)); // Neighborhood list
    while (c != '}')
    {
        // Condition indicating that the graph representation has started
        if (left == 1)
        {
            c = (char)getchar();

            // The condition meaning that we encountered an arrow and we want to skip these signs
            if (c == '-')
                c = (char)getchar();

            // Means that the name of the node began at the input
            else if (c != '}' && c != ' ' && c != '\n' && c != '>')
            {
                size_t length = 1;
                char *string = calloc(100, sizeof(char));
                read(&string, &length, &c);

                if (counter == 0)
                    copy(&v1, &string, &counter, &length);
                else
                    copy(&v2, &string, &counter, &length);

                // If the node has not appeared before then we save it in the table with node names
                if (numOfVertices == 0)
                    addVertex(&vertex, &adjList, &numOfVertices, &string, &length, &beginning, &exists);
                else
                {
                    repetition = 0;
                    for (size_t i = 0; i < numOfVertices; i++)
                    {
                        if (strcmp(vertex[i], string) == 0)
                            repetition = 1;
                    }
                    if (repetition == 0)
                    {
                        if (numOfVertices == memory)
                        {
                            memory *= 2;
                            vertex = realloc(vertex, memory * sizeof(char *));
                            adjList = realloc(adjList, memory * sizeof(newNode));
                        }
                        addVertex(&vertex, &adjList, &numOfVertices, &string, &length, &beginning, &exists);
                    }
                }
                // When we find two nodes, we connect them into neighbors
                if (counter == 2)
                {
                    find(&vertex, &numOfVertices, &v1, &v2, &a, &b);
                    addEdge(&adjList, a, b);
                    counter = 0;
                    free(v1);
                    free(v2);
                }
                free(string);
            }
        }
        else
            c = (char)getchar();
        if (c == '{')
            left = 1;
    }

    vertex = realloc(vertex, numOfVertices * sizeof(char *));
    adjList = realloc(adjList, numOfVertices * sizeof(newNode *));

    if (numOfVertices == 0 || exists == 0)
        printf("%d\n", 0);
    else
        printf("%d\n", BFS(beginning, numOfVertices, &adjList));

    for (size_t i = 0; i < numOfVertices; i++)
        free(vertex[i]);

    free(adjList);
    free(vertex);
}