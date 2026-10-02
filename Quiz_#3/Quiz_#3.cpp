#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;

// BÚSQUEDA BINARIA
// Complejidad: O(log n)

bool binarySearch(const vector<int>& arr, int target) {

    int left = 0;
    int right = static_cast<int>(arr.size()) - 1;

    while (left <= right) {

        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return true;
        }

        if (arr[mid] < target) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return false;
}


// MERGE
// Parte utilizada por Merge Sort

void merge(vector<int>& arr, int left, int mid, int right) {

    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1);
    vector<int> R(n2);

    // Copiar primera mitad
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }

    // Copiar segunda mitad
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = left;

    // Combinar ambas partes ordenadas
    while (i < n1 && j < n2) {

        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }

        k++;
    }

    // Elementos restantes de L
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Elementos restantes de R
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}


// MERGE SORT
// Complejidad: O(n log n)

void mergeSort(vector<int>& arr, int left, int right) {

    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    // Dividir primera mitad
    mergeSort(arr, left, mid);

    // Dividir segunda mitad
    mergeSort(arr, mid + 1, right);

    // Combinar
    merge(arr, left, mid, right);
}


// FUNCIÓN PARA VERIFICAR SI UN ARREGLO ESTÁ ORDENADO


bool isSorted(const vector<int>& arr) {

    for (size_t i = 1; i < arr.size(); i++) {

        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }

    return true;
}


// MAIN

int main() {

    // GENERADOR DE NÚMEROS ALEATORIOS

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> distrib(1, 100000000);


    // TAMAÑOS DE LOS ARREGLOS

    vector<int> sizes = {
        10000,
        50000,
        100000,
        500000,
        1000000,
        5000000
    };


    // CANTIDAD DE REPETICIONES

    const int BINARY_REPETITIONS = 10000;
    const int MERGE_REPETITIONS = 5;


    // ARCHIVO CSV

    ofstream csv("resultados.csv");

    if (!csv.is_open()) {

        cerr << "Error: no se pudo crear resultados.csv\n";

        return 1;
    }


    // Encabezados del CSV

    csv << "N,"
        << "BinarySearch_us,"
        << "log_N,"
        << "MergeSort_us,"
        << "N_log_N\n";

    // ENCABEZADO DEL PROGRAMA

    cout << "============================================================\n";
    cout << "              BENCHMARK COMPARATIVO\n";
    cout << "============================================================\n";

    cout << "Algoritmos evaluados:\n";
    cout << "1. Busqueda Binaria -> O(log n)\n";
    cout << "2. Merge Sort        -> O(n log n)\n\n";

    cout << "Repeticiones Binary Search: "
        << BINARY_REPETITIONS << "\n";

    cout << "Repeticiones Merge Sort: "
        << MERGE_REPETITIONS << "\n\n";


    // ENCABEZADO DE LA TABLA

    cout << fixed << setprecision(3);

    cout << setw(12) << "N"
        << setw(22) << "Binary (us)"
        << setw(18) << "log(N)"
        << setw(22) << "Merge (us)"
        << setw(22) << "N*log(N)"
        << "\n";

    cout << "-------------------------------------------------------------------------------\n";


    // PRUEBAS PARA CADA TAMAÑO

    for (int n : sizes) {

        cout << "Procesando N = " << n << "...\n";


        // 1. GENERAR ARREGLO ALEATORIO-

        vector<int> base_arr(n);

        for (int i = 0; i < n; i++) {

            base_arr[i] = distrib(gen);
        }


        // BÚSQUEDA BINARIA

        // Crear copia del arreglo
        vector<int> arr_bs = base_arr;


        // Binary Search necesita arreglo ordenado.
        // Este proceso NO se incluye en la medición.
        sort(arr_bs.begin(), arr_bs.end());


        // GENERAR LOS OBJETIVOS ANTES DE MEDIR

        vector<int> targets(BINARY_REPETITIONS);

        uniform_int_distribution<int> index_dist(0, n - 1);

        for (int i = 0; i < BINARY_REPETITIONS; i++) {

            targets[i] = base_arr[index_dist(gen)];
        }


        // MEDIR BÚSQUEDA BINARIA

        volatile int foundCount = 0;

        auto start_bs = chrono::high_resolution_clock::now();


        for (int i = 0; i < BINARY_REPETITIONS; i++) {

            bool found = binarySearch(arr_bs, targets[i]);

            if (found) {
                foundCount++;
            }
        }


        auto end_bs = chrono::high_resolution_clock::now();


        // Tiempo total en microsegundos
        auto duration_bs =
            chrono::duration_cast<chrono::microseconds>(
                end_bs - start_bs
            ).count();


        // Tiempo promedio
        double avg_binary_us =
            static_cast<double>(duration_bs)
            / BINARY_REPETITIONS;


        // MERGE SORT

        long long total_merge_us = 0;


        for (int repetition = 0;
            repetition < MERGE_REPETITIONS;
            repetition++) {


            // Crear copia del arreglo
            // FUERA DEL TIEMPO MEDIDO


            vector<int> arr_ms = base_arr;


            // MEDIR SOLAMENTE MERGE SORT

            auto start_ms =
                chrono::high_resolution_clock::now();


            mergeSort(arr_ms, 0, n - 1);


            auto end_ms =
                chrono::high_resolution_clock::now();


            // Tiempo en microsegundos

            auto duration_ms =
                chrono::duration_cast<chrono::microseconds>(
                    end_ms - start_ms
                ).count();


            total_merge_us += duration_ms;


            // VERIFICAR RESULTADO

            if (!isSorted(arr_ms)) {

                cerr << "\nERROR: Merge Sort no ordeno correctamente.\n";

                csv.close();

                return 1;
            }
        }


        // PROMEDIO MERGE SORT


        double avg_merge_us =
            static_cast<double>(total_merge_us)
            / MERGE_REPETITIONS;


        // VALORES TEÓRICOS


        double log_n = log(static_cast<double>(n));

        double n_log_n =
            static_cast<double>(n) * log_n;


        // MOSTRAR RESULTADOS EN CONSOLA

        cout << setw(12) << n
            << setw(22) << avg_binary_us
            << setw(18) << log_n
            << setw(22) << avg_merge_us
            << setw(22) << n_log_n
            << "\n";


        // GUARDAR RESULTADOS EN CSV

        csv << n << ","
            << avg_binary_us << ","
            << log_n << ","
            << avg_merge_us << ","
            << n_log_n
            << "\n";
    }


    // CERRAR CSV

    csv.close();


    // FINAL

    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nBenchmark finalizado correctamente.\n";

    cout << "Los resultados fueron guardados en:\n";
    cout << "resultados.csv\n";

    cout << "\nPuedes abrir resultados.csv en Excel para crear las graficas.\n";


    return 0;
}