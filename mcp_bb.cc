//Miguel Ferrer Castellá
//18071705F

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <tuple>
#include <cstring>
#include <climits>
#include <chrono>
#include <iomanip>
#include <algorithm> 

using namespace std;

// Define un valor centinela para inicializar matrices.
#define SENTINEL -1

// Estructura para representar un nodo en la búsqueda.
struct Node {
    int x, y; // Coordenadas del nodo.
    int cost; // Costo acumulado para llegar al nodo.
    int optimisticCost; // Costo optimista estimado desde el nodo hasta el destino.
    vector<pair<int, int>> path; // Camino recorrido hasta el nodo.
    
    // Sobrecarga del operador < para comparar nodos basándose en el costo optimista.
    bool operator<(const Node& other) const {
        return optimisticCost > other.optimisticCost;
    }
};

// Variables globales para estadísticas.
int nvisit = 0, nexplored = 0, nleaf = 0, nunfeasible = 0, 
nnot_promising = 0, npromising_but_discarded = 0, 
nbest_solution_updated_from_leafs = 0, 
nbest_solution_updated_from_pessimistic_bound = 0;

// Vector global para el mejor camino encontrado.
vector<pair<int, int>> best_path;

// Direcciones posibles de movimiento en la matriz.
int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

// Declaración de funciones.
void show_usage();
bool isValid(int x, int y, int n, int m);
int mcp_it_vector(int ,int , vector<vector<int>> &mat);
int mcp_it_vector(vector<int> &a, int ,int , vector<vector<int>> &mat);
int optimistic(int x1, int y1, int x2, int y2);
tuple<int, vector<pair<int, int>>, vector<int>> mcp_bb(int n, int m, vector<vector<int>> &grid);
void printPath2D(int n, int m, const vector<pair<int, int>>& path);
void printPath1D(const vector<pair<int, int>>& path);

int main(int argc, char* argv[]) {
    bool p = false;
    bool p2D = false;

    string file_name;

    // Procesamiento de argumentos de línea de comandos.
    for(int i = 1; i < argc ; i ++){
        string arg = argv [i];
        
        if(arg == "-f"){
            i ++;
            if(i >= argc) {
                cerr << "ERROR : missing filename." << endl;
                show_usage();
                exit(EXIT_FAILURE);
            }
            file_name = argv[i];
        }else if(arg == "-p"){
            p = true;
        }else if(arg == "--p2D"){
            p2D = true;
        }else {
            cerr << "ERROR : unknown option ’" << arg << "’." << endl;
            show_usage();
            exit (EXIT_FAILURE);
        }
    }

    // Verificación de que se haya proporcionado un nombre de archivo.
    if(file_name.empty()) {
        show_usage();
        exit (EXIT_FAILURE);
    }

    ifstream is (file_name);

    // Verificación de la apertura correcta del archivo.
    if (!is) {
        cerr << "ERROR : can’t open file: " << file_name << endl ;
        show_usage();
        exit(EXIT_FAILURE);
    }    

    int n, m;
    is >> n >> m;

    vector<vector<int>> grid(n, vector<int>(m));

    // Lectura de la matriz desde el archivo.
    for(int i = 0; i < n ; i++)
        for(int j = 0; j < m ; j++){
            is >> grid[i][j];
        } 

    // Inicio del temporizador.
    clock_t start = clock();
    auto result = mcp_bb(n, m, grid);
    clock_t end = clock();
    double duration = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    int bestW = get<0>(result);
    vector<pair<int, int>> best_path = get<1>(result);
    vector<int> stats = get<2>(result);
      
    // Impresión de resultados.
    cout << bestW << endl;
    for (int stat : stats) {
        cout << stat << " ";
    }
    cout << endl;
    cout << duration << endl;

    // Opcional: impresión del camino en formato 2D.
    if (p2D) {
        printPath2D(n, m, best_path);
        cout << bestW << endl;
    }

    // Opcional: impresión del camino en formato 1D.
    if (p) {
        printPath1D(best_path);
    }

    return 0;
}

// Función para mostrar la forma de uso del programa.
void show_usage(){
    cout << "Usage:"<< endl;
    cout << "mcp_bb [-p] [--p2D] -f filename\n" << endl;
}

// Función para verificar si una posición (x, y) es válida dentro de la matriz.
bool isValid(int x, int y, int n, int m) {
    return x >= 0 && y >= 0 && x < n && y < m;
}

// Función para calcular una matriz de costos pesimistas.
void Mpesimistic(vector<vector<int>> &M, vector<vector<int>> &mat, int n, int m) {
    // Inicializar la última celda con 0 ya que no hay costo para permanecer en la meta.
    M[n][m] = 0;

    // Inicializar el borde derecho y el borde inferior de la matriz.
    for (int i = n - 1; i >= 0; --i) {
        M[i][m] = mat[i][m] + M[i + 1][m];
    }
    for (int j = m - 1; j >= 0; --j) {
        M[n][j] = mat[n][j] + M[n][j + 1];
    }

    // Calcular el camino de menor dificultad para cada elemento de la matriz.
    for (int i = n - 1; i >= 0; --i) {
        for (int j = m - 1; j >= 0; --j) {
            int right = M[i][j + 1] + mat[i][j + 1];
            int down = M[i + 1][j] + mat[i + 1][j];
            int diag = M[i + 1][j + 1] + mat[i + 1][j + 1];
            M[i][j] = min({right, down, diag});
        }
    }
}

// Función para calcular el costo del camino mínimo utilizando una heurística iterativa.
int mcp_it_vector(int n,int m, vector<vector<int>> &mat){
    vector<int> a(m,SENTINEL);
    return mcp_it_vector(a,n,m,mat);
}

int mcp_it_vector(vector<int> &a, int n,int m, vector<vector<int>> &mat){

    a[0] = mat[0][0];

    for (int j = 1; j <m; ++j){
        a[j] = a[j - 1] + mat[0][j];
    }

    for (int i = 1; i < n; ++i){
        int diagonal = a[0];
        a[0] += mat[i][0];

        for (int j = 1; j < m; ++j) {
            int temp = a[j];

            a[j] = mat[i][j] + min(a[j],min(a[j-1],diagonal));

            diagonal = temp;
        }
    }

    return a[m-1];
}

// Función para calcular una estimación optimista del costo restante.
int optimistic(int x1, int y1, int x2, int y2) {
    return max(abs(x2 - x1), abs(y2 - y1));
}

// Función principal de la búsqueda en anchura con poda y vinculación.
tuple<int, vector<pair<int, int>>, vector<int>> mcp_bb(int n, int m, vector<vector<int>> &grid) {
    priority_queue<Node> pq;
    Node startNode = {0, 0, grid[0][0], grid[0][0] + optimistic(0, 0, n - 1, m - 1), {{0,0}}};
    pq.emplace(startNode);

    vector<vector<int>> cost(n, vector<int>(m, INT_MAX));
    cost[0][0] = grid[0][0];

    // Calcular cota pesimista inicial
    int best_cost = mcp_it_vector(n, m, grid)+1;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();
        ++nvisit;

        // Si el costo optimista del nodo actual es mayor o igual al mejor costo conocido, se descarta.
        if (current.optimisticCost >= best_cost) {
            ++npromising_but_discarded;
            continue;
        }

        // Si se llega a la meta, se actualiza el mejor camino si es necesario.
        if (current.x == n - 1 && current.y == m - 1) {
            nleaf++;
            if(current.cost < best_cost){
                best_path = current.path;
                best_cost = current.cost;
                cost[current.x][current.y] = current.cost;
                ++nbest_solution_updated_from_leafs;
            }
            continue;
        }

        // Expansión de los nodos vecinos.
        for (int i = 0; i < 8; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (isValid(nx, ny, n, m)) {
                nvisit++;

                int new_cost = current.cost + grid[nx][ny];
                int optimisticCost = new_cost + optimistic(nx, ny, n - 1, m - 1);

                // Si el costo optimista es mayor o igual al mejor costo conocido, se descarta.
                if(optimisticCost >= best_cost){
                    nnot_promising++;
                    continue;
                }

                // Si se encuentra un camino mejor, se actualizan los costos y se encola el nuevo nodo.
                if (optimisticCost < best_cost and new_cost < cost[nx][ny]) {
                    cost[nx][ny] = new_cost;
                    vector<pair<int, int>> new_path = current.path;
                    new_path.push_back({nx, ny});
                    Node nextNode = {nx, ny, new_cost, optimisticCost, new_path};
                    pq.emplace(nextNode);
                    ++nexplored;
                }
            } else {++nunfeasible;}
        }
    }
    return make_tuple(best_cost, best_path,
                      vector<int>{nvisit, nexplored, nleaf, nunfeasible, nnot_promising,
                                  npromising_but_discarded, nbest_solution_updated_from_leafs,
                                  nbest_solution_updated_from_pessimistic_bound});
}

// Función para imprimir el camino en formato 2D.
void printPath2D(int n, int m, const vector<pair<int, int>>& path) {
    vector<vector<char>> display(n, vector<char>(m, '.'));
    for (const auto& p : path) {
        display[p.first][p.second] = 'x';
    }
    for (const auto& row : display) {
        for (char cell : row) {
            cout << cell;
        }
        cout << endl;
    }
}

// Función para imprimir el camino en formato 1D.
void printPath1D(const vector<pair<int, int>>& path) {
    cout << "<";
    for (size_t i = 1; i < path.size(); ++i) {
        int dx = path[i].first - path[i-1].first;
        int dy = path[i].second - path[i-1].second;
        if (dx == -1 && dy == -1) cout << "8";
        else if (dx == -1 && dy == 0) cout << "1";
        else if (dx == -1 && dy == 1) cout << "2";
        else if (dx == 0 && dy == 1) cout << "3";
        else if (dx == 1 && dy == 1) cout << "4";
        else if (dx == 1 && dy == 0) cout << "5";
        else if (dx == 1 && dy == -1) cout << "6";
        else if (dx == 0 && dy == -1) cout << "7";
    }
    cout << ">" << endl;
}
