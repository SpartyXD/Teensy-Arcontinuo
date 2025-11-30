#include <Arduino.h>
#include "BlobSender.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include <vector>
#include <cstring>
#include <map>
#include <queue>
#include <cmath>


struct BlobPosition {
    float x;
    float y;
    int missedFrames;
};

std::map<int, BlobPosition> trackedBlobs;
std::map<int, BlobPosition> recentlyLostBlobs;
int nextAvailableID = 1;
std::queue<int> availableIDs;

struct DetectedBlob {
    int x_center;
    int y_center;
    int count;
    int pressure;
    int assignedId;

    DetectedBlob(int x, int y, int c, int p)
        : x_center(x), y_center(y), count(c), pressure(p), assignedId(-1) {}
};


const float maxAllowedMovement =2;  
const int maxMissedFrames = 5;
#define RST 1
#define DATA 2
#define FQ  3
#define CLK 38

unsigned long freq = 12000000UL;

// Definiciones para la matriz
const int M = 64;          // Número de filas de la matriz
const int N = 13;          // Número de columnas de la matriz + 1

#define MATRIX_ROWS 64
#define MATRIX_COLS 13
#define MAX_BLOBS 10

//53 -> 53
//11 -> 12

//64 -> 64
//12 -> 13

char output[256];

// Parámetros ajustables
int threshold_min = 2;
int threshold_max = 255;
int min_area = 2;
int max_area = 200;
int numBlobs = 0;

// Intancia del gestor de envío de blobs
BlobSender blobSender(Serial1, 300000);

// Matriz global para el procesamiento
int globalMatrix[M][N];
SemaphoreHandle_t matrixMutex;

// Variable para habilitar/deshabilitar debug
bool debugEnabled = true;

// Flags para activar/desactivar filtros y binarización
bool lowPassFilterEnabled = false;
bool binarizationEnabled = false;

// Definiciones DMA
#define UART_NUM UART_NUM_2
#define BUF_SIZE (4096)
#define TXD_PIN (10)
#define RXD_PIN (9)

// Pin para la señal PWM
//#define PWM_PIN GPIO_NUM_18
#define PWM_PIN 38

// Matriz para la imagen binaria
bool binaryImage[M][N];

// Función para aplicar el filtro de paso bajo
void lowPassFilter(int input[M][N], int output[M][N]) {
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N-1; x++) {
            int sum = 0;
            int count = 0;

            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;

                    if (ny < M && nx < N) {
                        sum += input[ny][nx];
                        count++;
                    }
                }
            }

            output[y][x] = sum / count;
        }
    }
}

// Función para normalizar la matriz a valores entre 0 y 255
void normalizeMatrix(int matrix[M][N], int output[M][N]) {
    const int minVal = 0;
    const int maxVal = 512;

    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            output[y][x] = 255 * (matrix[y][x] - minVal) / (maxVal - minVal);
        }
    }
}

// Función para binarizar la imagen
void binarizeImage(int grayImage[M][N], bool binaryImage[M][N]) {
    for (int y = 0; y < M; y++) {
        for (int x = 0; x < N; x++) {
            // Aquí asumimos un umbral fijo para la binarización
            int threshold = 40; // Ajusta el valor del umbral según tus necesidades
            binaryImage[y][x] = grayImage[y][x] > threshold;
        }
    }
}

void printMatrix(int matrix[M][N]){
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            Serial.print(matrix[i][j]);
            if (j < N - 1) {
                Serial.print(",");
            }
        }
        Serial.println();
    }
}

void printBinaryImage(bool binaryImage[M][N]){
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            Serial.print(binaryImage[i][j] ? "1" : "0");
            if (j < N - 1) {
                Serial.print(",");
            }
        }
        Serial.println();
    }
}

void readSerialData(void *parameter) {
    int matrix[64][13];  // Matriz de 30 filas y 11 columnas (incluyendo el índice)
    int row = 0;
    int col = 0;
    bool startReading = false;  // Flag para indicar cuándo empezar a leer

    while (true) {
        uint8_t byteReceived;

        // Leer un byte desde el UART
        if (uart_read_bytes(UART_NUM, &byteReceived, 1, portMAX_DELAY) > 0) {
            if (byteReceived == 0xFF) {
                // Byte especial indicando el comienzo de una nueva fila
                if (debugEnabled) {
                    Serial.println("Byte de inicio recibido, esperando nueva fila");
                }
                startReading = true;  // Comenzar a leer la nueva fila
                continue;
            }

            if (startReading) {
                // Leer el índice de la fila (primera columna)
                row = byteReceived;
                col = 0;  // Reiniciar columna para la nueva fila

                if (debugEnabled) {
                    Serial.print("Inicio de fila: ");
                    Serial.println(row);
                }

                startReading = false;  // Preparado para leer los valores de la fila
                continue;
            }

            // Guardar el byte recibido en la matriz
            if (col < 13) {
                matrix[row][col] = byteReceived;

                // Mensaje de depuración para verificar los valores recibidos
                if (debugEnabled) {
                    Serial.print("Row: ");
                    Serial.print(row);
                    Serial.print(", Col: ");
                    Serial.print(col);
                    Serial.print(", Value: ");
                    Serial.println(byteReceived);
                }

                col++;

                // Verificar si hemos completado la lectura de una fila
                if (col == 13) {
                    // Copiar la fila recibida a la matriz global
                    xSemaphoreTake(matrixMutex, portMAX_DELAY);
                    memcpy(globalMatrix[row], matrix[row], sizeof(matrix[row]));
                    xSemaphoreGive(matrixMutex);

                    if (debugEnabled) {
                        Serial.print("Fila ");
                        Serial.print(row);
                        Serial.println(" copiada a la matriz global");
                    }
                    col = 0;  // Reiniciar columna para la próxima fila
                    row = (row + 1) % 64;  // Incrementar la fila y asegurarse de que esté dentro de los límites
                }
            }
        }
    }
}

float distance(int x1, int y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void detectBlobs(int matrix[MATRIX_ROWS][MATRIX_COLS], Blob blobs[], int &blob_count) {
    static bool visited[MATRIX_ROWS][MATRIX_COLS];  
    memset(visited, 0, sizeof(visited));  
    blob_count = 0;

    int stack[MATRIX_ROWS * MATRIX_COLS][2];
    int stack_size = 0;

    std::map<int, bool> assignedThisFrame;
    std::vector<DetectedBlob> detectedBlobs;

    for (int y = 0; y < MATRIX_ROWS; y++) {
        for (int x = 0; x < MATRIX_COLS; x++) {
            if (visited[y][x]) continue;
            int value = matrix[y][x];
            if (value < threshold_min || value > threshold_max) continue;

            Blob blob = {};
            stack_size = 0;
            stack[stack_size][0] = x;
            stack[stack_size][1] = y;
            stack_size++;
            visited[y][x] = true;

            while (stack_size > 0) {
                stack_size--;
                int cx = stack[stack_size][0], cy = stack[stack_size][1];
                blob.x_sum += cx; blob.y_sum += cy;
                blob.count++; blob.pressure += matrix[cy][cx];

                constexpr int dx[8] = {-1,1,0,0,-1,-1,1,1};
                constexpr int dy[8] = {0,0,-1,1,-1,1,-1,1};

                for(int i=0; i<8; i++){
                    int nx=cx+dx[i], ny=cy+dy[i];
                    if(nx>=0 && ny>=0 && nx<MATRIX_COLS && ny<MATRIX_ROWS && !visited[ny][nx]){
                        int neighbor_value=matrix[ny][nx];
                        if(neighbor_value>=threshold_min && neighbor_value<=threshold_max){
                            stack[stack_size][0]=nx;
                            stack[stack_size][1]=ny;
                            stack_size++;
                            visited[ny][nx]=true;
                        }
                    }
                }
            }

            if(blob.count >= min_area && blob.count <= max_area){
                int x_center = blob.x_sum / blob.count;
                int y_center = blob.y_sum / blob.count;
                detectedBlobs.push_back(DetectedBlob(x_center, y_center, blob.count, blob.pressure));
            }
        }
    }

    // Asignación de IDs
    for (size_t i = 0; i < detectedBlobs.size(); i++) {
        DetectedBlob &detected = detectedBlobs[i];
        int assignedId = -1;
        float minDistance = maxAllowedMovement;

        // Intentar emparejar con blobs activos
        for (auto it = trackedBlobs.begin(); it != trackedBlobs.end(); ++it) {
            int id = it->first;
            BlobPosition &position = it->second;
            if (assignedThisFrame[id]) continue;
            float dist = distance(detected.x_center, detected.y_center, position.x, position.y);
            if (dist < minDistance) {
                minDistance = dist;
                assignedId = id;
            }
        }

        // Intentar recuperar un ID recientemente perdido SOLO si está MUY cerca
        if (assignedId == -1) {
            for (auto it = recentlyLostBlobs.begin(); it != recentlyLostBlobs.end(); ++it) {
                float dist = distance(detected.x_center, detected.y_center, it->second.x, it->second.y);
                if (dist < maxAllowedMovement / 2) { 
                    assignedId = it->first;
                    recentlyLostBlobs.erase(it);
                    break;
                }
            }
        }

        // Si aún no tiene ID, asignar uno de la cola si está disponible
        if (assignedId == -1) {
            if (!availableIDs.empty()) {
                assignedId = availableIDs.front();
                availableIDs.pop();
            }else {
                assignedId = nextAvailableID++;
        }
}

        detected.assignedId = assignedId;
        assignedThisFrame[assignedId] = true;
    }

    blob_count = 0;
    for (size_t i = 0; i < detectedBlobs.size() && blob_count < MAX_BLOBS; i++) {
        DetectedBlob &detectedBlob = detectedBlobs[i];
        blobs[blob_count].id = detectedBlob.assignedId;
        blobs[blob_count].x_sum = detectedBlob.x_center;
        blobs[blob_count].y_sum = detectedBlob.y_center;
        blobs[blob_count].pressure = detectedBlob.pressure;
        blobs[blob_count].count = detectedBlob.count;

        trackedBlobs[detectedBlob.assignedId] = {static_cast<float>(detectedBlob.x_center), static_cast<float>(detectedBlob.y_center), 0};

        blob_count++;
    }

    // Manejo de blobs desaparecidos
    for (auto it = trackedBlobs.begin(); it != trackedBlobs.end();) {
        if (!assignedThisFrame[it->first]) {
            it->second.missedFrames++;
            if (it->second.missedFrames > maxMissedFrames) {
                availableIDs.push(it->first);  // Liberar el ID para su reutilización
                recentlyLostBlobs[it->first] = it->second;  
                it = trackedBlobs.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }

    // Si no hay blobs, reiniciar los IDs
    if (trackedBlobs.empty()) {
        nextAvailableID = 1;
        availableIDs = std::queue<int>(); // Vaciar la cola
        recentlyLostBlobs.clear();
    }
    
    // Debug actualizado (opcional)
    /*
    Serial.print("DEBUG: ");
    for (int i = 0; i < blob_count && i < 2; i++) {  
        Serial.print("[ID:"); Serial.print(blobs[i].id);
        Serial.print(" X:"); Serial.print(blobs[i].x_sum);
        Serial.print(" Y:"); Serial.print(blobs[i].y_sum);
        Serial.print(" Z:"); Serial.print(blobs[i].pressure);
        Serial.print("]  ");
    }
    Serial.println();
    */
}

void processMatrixTask(void *parameter) {
    int filteredMatrix[M][N];
    int normalizedMatrix[M][N];

    while (true) {
        xSemaphoreTake(matrixMutex, portMAX_DELAY);
        int processingMatrix[M][N];
        memcpy(processingMatrix, globalMatrix, sizeof(globalMatrix));
        xSemaphoreGive(matrixMutex);

        //lowPassFilter(processingMatrix, filteredMatrix);
        //normalizeMatrix(filteredMatrix, normalizedMatrix);
        //printMatrix(normalizedMatrix);
        // Imprimir la matriz promedio/filtrada/normalizada para verificar
        //Serial.println("Matriz Normalizada:");
        printMatrix(globalMatrix);
        
        // Blob detection -> Reciclado
        //Blob blobs[MAX_BLOBS] = {};
        //detectBlobs(normalizedMatrix, blobs, numBlobs);  // Detectar blobs en la matriz táctil
        //blobSender.sendBlobData(blobs, numBlobs);  // Enviar blobs por UART

        //Serial.println(output);

        // Imprimir la imagen binaria si está habilitada
        //if (binarizationEnabled) {
        //    Serial.println("Imagen Binaria:");
        //    printBinaryImage(binaryImage);
        //}

        vTaskDelay(pdMS_TO_TICKS(5)); // Espera 1 segundo entre impresiones
    }
}

void setup() {

    Serial.begin(300000); // Para visualizar la matriz en la consola serial
    Serial.println("Iniciando todo :D");

    // Configurar UART con DMA
    const uart_config_t uart_config = {
        .baud_rate = 300000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
    uart_enable_pattern_det_baud_intr(UART_NUM, '\n', 1, 10000, 10, 10);
    uart_pattern_queue_reset(UART_NUM, 20);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_SPEED_MODE_MAX,  // Modo de alta velocidad
        .duty_resolution = LEDC_TIMER_2_BIT, // Resolución del duty de 2 bits
        .timer_num = LEDC_TIMER_0,           // Timer 0
        .freq_hz = 10000000,                 // Frecuencia PWM de 10 MHz
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num   = PWM_PIN,
        .speed_mode = LEDC_SPEED_MODE_MAX,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 2,              // Ciclo de trabajo al 25% (1/4)
        .hpoint     = 0
    };
    
    ledc_channel_config(&ledc_channel);

    // Habilitar PWM
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, ledc_channel.duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    

    // Crear el mutex para proteger la matriz
    matrixMutex = xSemaphoreCreateMutex();
    if (matrixMutex == NULL) {
        Serial.println("Error creando el mutex");
        while (true);
    }

    blobSender.begin();

    // Crear tareas en núcleos específicos
    xTaskCreatePinnedToCore(
        readSerialData,     /* Función de la tarea */
        "ReadSerialData",   /* Nombre de la tarea */
        18192,               /* Tamaño de la pila en palabras */
        NULL,               /* Parámetro de entrada */
        1,                  /* Prioridad de la tarea */
        NULL,               /* Manejador de la tarea */
        0);                 /* Núcleo en el que se ejecutará la tarea */

    xTaskCreatePinnedToCore(
        processMatrixTask,  /* Función de la tarea */
        "ProcessMatrixTask",/* Nombre de la tarea */
        18192,               /* Tamaño de la pila en palabras */
        NULL,               /* Parámetro de entrada */
        1,                  /* Prioridad de la tarea */
        NULL,               /* Manejador de la tarea */
        1);                 /* Núcleo en el que se ejecutará la tarea */
}

void loop() {
    // No se usa en este caso ya que estamos usando tareas FreeRTOS
}