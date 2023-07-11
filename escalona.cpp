/**
 * \name Escalonamento
 * \mainpage Documentação do programa Escalonamento
 * 
 * \section Equipe
 * Victor Matheus Alflen - GRR20182599<br>
 * Wendel Caio Moro - GRR20182641
 * 
 * 
 * \section Utilização
 * Pode-se executar o programa com os seguintes comandos:<br><br>
 * make<br>
 * ./escalona < exemplo.in
 * 
*/

/**
 * \file
 * \brief Contém a função main que, ao ser executada, recebe um conjunto de instruções, e partir delas, monta agendas e verifica se é possível
 * a serialização e se existem visões equivalentes
 */

#include <bits/stdc++.h>
using namespace std;

/**
 * \struct instruction
 * Struct que define a instrução
 */
struct instruction {
    int timestamp;
    int transaction;
    char operation;
    char attribute;
};
bool operator==(instruction a, instruction b) {
    return a.timestamp == b.timestamp && a.transaction == b.transaction && a.operation == b.operation && a.attribute == b.attribute;
}
bool operator!=(instruction a, instruction b) {
    return !(a.timestamp == b.timestamp && a.transaction == b.transaction && a.operation == b.operation && a.attribute == b.attribute);
}
bool operator<(instruction a, instruction b) {
    if (a.timestamp > b.timestamp) return false;
    if (a.timestamp < b.timestamp) return true;
    if (a.transaction > b.transaction) return false;
    if (a.transaction < b.transaction) return true;
    if (a.operation > b.operation) return false;
    if (a.operation < b.operation) return true;
    if (a.attribute > b.attribute) return false;
    if (a.attribute < b.attribute) return true;
    return false;
}

// structure for graph node
struct node {
    set<int> edges;
};

vector<instruction> input;
vector<bool> visited;
vector<bool> transactions;

// vetor de outputs(identificador, quais transações fizeram parte, serialização, equivalência)
vector<tuple<int, vector<int>, string, string>> output;

/**
 * Comparador customizado para ordenar por timestamps
 */
bool sortInput(const instruction& a, const instruction& b) {
    return (a.timestamp < b.timestamp);
}

/**
 * Exibe a agenda, usado apenas para debug
 */
void printAgenda (vector<instruction> &agenda) {
    cout << "Agenda: " << endl;
    for (size_t i = 0; i < agenda.size(); i++) {
        cout << setw(2) << setfill(' ') << agenda[i].timestamp << ' ' << agenda[i].transaction << ' ' << agenda[i].operation << ' ' << agenda[i].attribute << endl;
    }
    cout << endl;
}

/**
 * Exibe a instrução, usado apenas para debug
 */
void printInst (instruction ins) {
    cout << setw(2) << setfill(' ') << ins.timestamp << ' ' << ins.transaction << ' ' << ins.operation << ' ' << ins.attribute;
}

/**
 * Função auxiliar para a função isCyclic
 * @param v  Um índice que indica qual vértice está a ser analisado
 * @param visited  Um vetor booleano, que guarda quais vértices já foram visitados
 * @param recStack  Um vetor para auxiliar na pilha de recursão para analisar vértices adjacentes de v
 * @param graph  Um vetor de vértices de um grafo direcionado
 * @return true Se todos os vértices adjacentes apontados pelo vetor de recursão forem marcados como true, false caso contrário
 */
bool isCyclicUtil(int v, bool visited[], bool *recStack, vector <node> &graph) {
    if(visited[v] == false) {
        // Mark the current node as visited and part of recursion stack
        visited[v] = true;
        recStack[v] = true;
 
        // Recur for all the vertices adjacent to this vertex
        for (int edge : graph[v].edges) {
            if ( !visited[edge] && isCyclicUtil(edge, visited, recStack, graph) ) {
                return true;
            }
            else if (recStack[edge]) {
                return true;
            }
        }
 
    }
    recStack[v] = false;  // remove the vertex from recursion stack

    return false;
}
 
/**
 * Verifica se um grafo contém ciclos
 * @param graph Um vetor de vértices de um grafo direcionado
 * @return true Se o grafo conter ciclos, false caso contrário
 */
bool isCyclic(vector <node> &graph) {
    // Mark all the vertices as not visited and not part of recursion stack
    bool *visited = new bool[graph.size()];
    bool *recStack = new bool[graph.size()];
    for(size_t i = 1; i < graph.size(); i++) {
        visited[i] = false;
        recStack[i] = false;
    }
 
    // Call the recursive helper function to detect cycle in different
    // DFS trees
    for(size_t i = 1; i < graph.size(); i++) {
        if ( !visited[i] && isCyclicUtil(i, visited, recStack, graph)) {
            return true;
        }
    }
 
    return false;
}

/**
 * Cria uma agenda a partir do vetor global 'input'
 * @param start O indíce da primeira instrução da agenda a partir de um vetor de Instruções
 * @return A agenda
 */
vector<instruction> create_agenda(int start) {
    set<int> transactions;
    transactions.insert(input[start].transaction);
    vector<instruction> agenda;

    int index = start;
    while (!transactions.empty()) {
        if (input[index].operation == 'C') {
            transactions.erase(input[index].transaction);
        } else {
            transactions.insert(input[index].transaction);
        }
        agenda.push_back(input[index]);
        index++;
    }

    return agenda;
}

/**
 * Testa a seriabilidade de uma agenda
 * @param agenda A agenda da qual será derivada uma nova com as intruções ordenadas por transação
 * @return true Se é a agenda é serializável, false caso contrário
 */
bool test_seriability(vector<instruction> &agenda) {
    set<int> transactions;
    set<char> attributes;

    for (size_t i = 0; i < agenda.size(); i++) {
        transactions.insert(agenda[i].transaction);
        attributes.insert(agenda[i].attribute);
    }

    // create the graph    
    int greater = 0;
    for (int trans : transactions) {
        if (trans > greater) {
            greater = trans;
        }
    }

    vector<node> graph(greater + 1);

    // look for dependencies for each attribute
    for (char attr :  attributes) {
        for (int j = agenda.size() - 1; j >= 0; j--) {
            // If is not the attribute of iteration, go to next it
            if (agenda[j].attribute != attr) {
                continue;
            }

            for (int i = j - 1; i >= 0; i--) {
                // If is same transaction or Different attributes, go to next it
                if (agenda[i].transaction == agenda[j].transaction || agenda[i].attribute != agenda[j].attribute) {
                    continue;
                }

                if (agenda[j].operation == 'R') {                     
                    // Reading after write
                    if (agenda[i].operation == 'W') {
                        graph[agenda[i].transaction].edges.insert(agenda[j].transaction);
                    }                
                }                
                else if (agenda[j].operation == 'W') {
                    // Writing after Read
                    if (agenda[i].operation == 'R') {
                        graph[agenda[i].transaction].edges.insert(agenda[j].transaction);
                    }
                    // Writing after Write
                    else if (agenda[i].operation == 'W') {
                        graph[agenda[i].transaction].edges.insert(agenda[j].transaction);
                    }
                }
            }
        }
    }

    // Check if has a cycle
    bool hasCycle = isCyclic(graph);

    return !hasCycle;
}

/**
 * Cria uma nova agenda com as instruções ordenadas por transactions
 * @param agenda A agenda da qual será derivada uma nova com as intruções ordenadas por transação
 * @param transactions A ordem na qual as transações devem aparecer na agenda
 * @return A agenda ordenada
 */
vector<instruction> order_agenda(vector<instruction> &agenda, vector<int> &transactions) {
    vector<instruction> new_agenda;
    for (int t : transactions) {
        for (instruction i : agenda) {
            if (i.transaction == t) {
                new_agenda.push_back(i);
            }
        }
    }

    return new_agenda;
}

/**
 * Testa todas as visões para uma agenda. Para a execução quando detectar uma agenda
 * equivalente
 * @param agenda A agenda para qual as visões serão testadas
 * @return true se a agenda possui uma visão equivalente, false caso contrário
 */
bool test_views(vector<instruction> &agenda) {
    set<int> attributes;
    set<int> transactions;

    for (size_t i = 0; i < agenda.size(); i++) {
        attributes.insert(agenda[i].attribute);
        transactions.insert(agenda[i].transaction);
    }

    // última escrita no atributo
    map<char, instruction> last_write;      // última instrução que editou o atributo
    map<instruction, instruction> base_dep;      // dependências de leitura

    for (instruction i : agenda) {
        if (i.operation == 'W') {
            last_write[i.attribute] = i;
        } else if (i.operation == 'R') {
            base_dep[i] = last_write[i.attribute];
        }
    }

    bool equivalent = false;

    vector<int> transac(transactions.begin(), transactions.end());
    do  {
        vector<instruction> permu = order_agenda(agenda, transac);
    
        map<char, instruction> actual_write;      // última instrução que editou o atributo
        map<instruction, instruction> permu_dep;      // dependências de leitura

        for (instruction i : permu) {
            if (i.operation == 'W') {
                actual_write[i.attribute] = i;
            } else if (i.operation == 'R') {
                permu_dep[i] = actual_write[i.attribute];
            }
        }

        bool flag = true;
        for (pair<instruction, instruction> dep : base_dep) {
            if (base_dep[dep.first] != permu_dep[dep.first]) {
                flag = false;
            }
        }
        for (pair<char, instruction> last : last_write) {
            if (last_write[last.first] != actual_write[last.first]) {
                flag = false;
            }
        }

        equivalent = equivalent || flag;
        if (equivalent) {
            return true;
        }

    } while (next_permutation(transac.begin(), transac.end()));

    return false;
}


/**
 * Função principal
 */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    // Read all the instructions
    instruction ins;
    while (cin >> ins.timestamp) {
        cin >> ins.transaction >> ins.operation >> ins.attribute;
        input.push_back(ins);
    }
    
    sort(input.begin(), input.end(), sortInput);
    visited.resize(input.size(), false);

    int id = 1;
    for (size_t i = 0; i < input.size(); ) {

        vector<instruction> agenda = create_agenda(i);

        set<int> transactions;
        for (size_t i = 0; i < agenda.size(); i++) {
            transactions.insert(agenda[i].transaction);
        }

        bool serializable = test_seriability(agenda);
        bool equivalent = test_views(agenda);

        i += agenda.size();

        cout << id << " ";
        int counter = transactions.size();
        for (int t : transactions) {
            if (counter == 1) {
                cout << t;
            } else {
                cout << t << ",";
            }
            counter--;
        }
        cout << " ";

        if (serializable) {
            cout << "SS ";
        } else {
            cout << "NS ";
        }

        if (equivalent) {
            cout << "SV";
        } else {
            cout << "NV";
        }
        cout << endl;
        id++;
    }

    return 0;
}