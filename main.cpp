#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <limits>
#include <queue>
#include <random>
#include <utility>
#include <vector>

using namespace std;

using number = double; 
using Clock = chrono::high_resolution_clock;
const number infinity = numeric_limits<number>::infinity(); 

int n; 
vector<pair<number, number>> city; //number of cities and their coordinates
vector<vector<number>> W;
long long num_nodes; // Number of nodes in the search tree
// Node structure for the search tree
struct node {
    int level; // Level of the node in the tree
    vector<int> path; // Path taken to reach this node
    number bound; 
};

struct compareBound {
    bool operator()(const node& a, const node& b) // Comparator for the priority queue to order nodes by their bound
    {
        return a.bound > b.bound; // Min-heap based on bound
    }
};

using priority_queue_node = priority_queue<node, vector<node>, compareBound>; // Priority queue to store nodes based on their bound.

void initialize(priority_queue_node& pq) {
    while(!pq.empty()) 
    {
        pq.pop(); 
    }
}

bool empty(const priority_queue_node& pq) {
    return pq.empty(); 
}

void insert(priority_queue_node& pq, const node& v) {
    pq.push(v); // Insert a node into the priority queue
}
void remove(priority_queue_node& pq, node& v) {
        v = pq.top(); // Get the node with the smallest bound
        pq.pop(); // Remove it from the priority queue
}

bool member(int i, const vector<int>& path) {
    return find(path.begin(), path.end(), i) != path.end(); 
}
void cities(int temp, unsigned seed, number side = 1000.0) {
    n = temp; 
    city.assign(n + 1, {0.0, 0.0}); // Initialize city coordinates
    mt19937 rng(seed); // Random number generator
    uniform_real_distribution<number> dist(0.0, side); // Distribution for city coordinates
    for (int i = 1; i <= n; ++i) {
        city[i] = {dist(rng), dist(rng)}; // Assign random coordinates
    }
}

void matrix() {
    W.assign(n + 1, vector<number>(n + 1, 0.0)); // Initialize distance matrix
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (i == j) {
                W[i][j] = infinity; // Distance from a city to itself is infinity
            } else {
                W[i][j]=hypot(city[i].first - city[j].first, city[i].second - city[j].second); // Calculate Euclidean distance
            }
        }
    }
}

number length(const node& u)
{
    number total_length = 0.0;
    for (size_t i = 0; i + 1 < u.path.size(); i++) {
        total_length += W[u.path[i]][u.path[i +1]]; // Sum the distances along the path
    }
    return total_length;
}

number bound(const node& v) {
    vector<char> on_path(n + 1, 0);

    for (int x : v.path) {
        on_path[x] = 1;
    }

    int start = v.path.front();
    int last = v.path.back();

    number total = 0.0;

    // Add the cost of the edges already chosen
    for (size_t i = 0; i + 1 < v.path.size(); i++) {
        total += W[v.path[i]][v.path[i + 1]];
    }

    // Add the cheapest outgoing edge from the last city
    number min_out = infinity;

    for (int j = 1; j <= n; j++) {
        if (!on_path[j]) {
            min_out = min(min_out, W[last][j]);
        }
    }

    total += min_out;

    // Add the cheapest outgoing edge from each unvisited city
    for (int i = 1; i <= n; i++) {
        if (!on_path[i]) {
            number cheapest = infinity;

            for (int j = 1; j <= n; j++) {
                if (j != i && (!on_path[j] || j == start)) {
                    if (W[i][j] < cheapest) {
                        cheapest = W[i][j];
                    }
                }
            }

            total += cheapest;
        }
    }

    return total;
}
int not_visited(const vector<int>& path) {
    for (int i = 1; i <= n; ++i) {
        if (!member(i, path)) {
            return i; 
        }
    }
    return -1; 
}

void traveling_salesman(vector<int>& best_path, number& best_length) {
    priority_queue_node pq; // Priority queue to store nodes based on their bound
    node u, v; // Nodes for the search tree
    num_nodes = 0; 
    initialize(pq); 
    v.level = 0; 
    v.path = {1}; 
    v.bound = bound(v); // Calculate the initial bound
    best_length = infinity; 
    insert(pq, v); // Insert the initial node into the priority queue
    while (!empty(pq)) {
        remove(pq, v); // Get the node with the smallest bound
        if (v.bound >= best_length) 
        { 
           continue;
        }
        num_nodes++; 
        u.level = v.level + 1; // Move to the next level

            for (int i = 2; i <= n; ++i) 
            {
                if (!member(i, v.path)) { // If city i is not in the current path
                    u.path = v.path; 
                    u.path.push_back(i); // Add city i to the path
                    if (u.level == n - 2) { 
                        u.path.push_back(not_visited(u.path));
                        u.path.push_back(1); // Return to the starting city
                        number current_length = length(u); // Calculate the length of this path
                        if (current_length < best_length) { // If this path is better than the best found update the best length
                            best_length = current_length; 
                            best_path = u.path; 
                    } 
                    else 
                    {
                        u.bound = bound(u); 
                        if (u.bound < best_length) { // If this node has a better bound than the best length found
                            insert(pq, u); 
                        }
                    }
                }
            }
        }
    }
    
    void output(const vector<int>& best_path, number best_length, number seconds)
{
    cout << "\nBest Tour: ";

    for (size_t i = 0; i < best_path.size(); i++) {
        cout << best_path[i];

        if (i + 1 < best_path.size()) {
            cout << " -> ";
        }
    }
    cout << fixed << setprecision(2);
    cout << "\nCost: " << best_length;

    cout << fixed << setprecision(6);
    cout << "\nTime: " << seconds << " seconds\n";
}

int main() {
    vector<int> best_path;
    number best_length;

   
    cities(10, 100);
    matrix();

    auto start = Clock::now();

    traveling_salesman(best_path,best_length);

    auto finish = Clock::now();

    number seconds = chrono::duration<number>(finish - start).count();

    cout << "TSP Branch and Bound Result\n";
    cout << "Number of cities: 10\n";

    output(best_path, best_length, seconds);

    // Table for the report
    cout << "\nResults Table\n";
    cout << "# Cities\tTime (sec.)\tCost\n";

    int sizes[] = {4, 5, 6, 7, 8, 9, 10, 11, 12};

    for (int size : sizes) {
        cities(size, 100 + size);
        matrix();

        auto t1 = Clock::now();

        traveling_salesman(best_path, best_length);

        auto t2 = Clock::now();

        number run_time = chrono::duration<number>(t2 - t1).count();

        cout << fixed << setprecision(6);
        cout << size << "\t\t" << run_time << "\t";

        cout << fixed << setprecision(2);
        cout << best_length << "*\n";
    }

    cout << "\n* = optimal tour found\n";

    return 0;
}
    
