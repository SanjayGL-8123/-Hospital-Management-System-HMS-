/**
 * ===================================================================
 *  HOSPITAL MANAGEMENT SYSTEM - PROFESSIONAL C BACKEND & DATABASE
 * ===================================================================
 *  Features:
 *   1. Persistent Binary File Database (hospital.db)
 *   2. Patient Registration, Search, Update & Tabular Display
 *   3. Interactive CLI Console Dashboard
 *   4. Integrated Winsock HTTP Web Server & REST API (Port 8080)
 * ===================================================================
 */

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#if defined(__has_include)
  #if __has_include(<stdio.h>)
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <time.h>
  #else
    #include <stddef.h>
    typedef struct _iobuf FILE;
    extern FILE *fopen(const char *, const char *);
    extern int fclose(FILE *);
    extern size_t fread(void *, size_t, size_t, FILE *);
    extern size_t fwrite(const void *, size_t, size_t, FILE *);
    extern int fprintf(FILE *, const char *, ...);
    extern int fputs(const char *, FILE *);
    extern int snprintf(char *, size_t, const char *, ...);
    extern int printf(const char *, ...);
    extern char *fgets(char *, int, FILE *);
    extern int fseek(FILE *, long, int);
    extern long ftell(FILE *);
    extern void *malloc(size_t);
    extern void free(void *);
    extern int atoi(const char *);
    extern size_t strlen(const char *);
    extern char *strcpy(char *, const char *);
    extern char *strncpy(char *, const char *, size_t);
    extern char *strstr(const char *, const char *);
    extern int strcmp(const char *, const char *);
    extern int strncmp(const char *, const char *, size_t);
    extern size_t strcspn(const char *, const char *);
    extern int tolower(int);
    extern int sscanf(const char *, const char *, ...);
    #define SEEK_SET 0
    #define SEEK_END 2
  #endif
  #if __has_include(<winsock2.h>)
    #include <winsock2.h>
    #include <ws2tcpip.h>
  #endif
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include <time.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif

#define MAX_PATIENTS 1000
#define DB_FILE "hospital.db"
#define PORT 8080
#define BUFFER_SIZE 65536

// Patient record data structure
typedef struct {
    int id;
    char name[64];
    int age;
    char disease[64];
    char doctor[64];
    char status[24]; // "Admitted", "Under Observation", "Discharged"
} Patient;

static Patient patients[MAX_PATIENTS];
static int patient_count = 0;

// Default 10 Seed Patients
static const Patient SEED_PATIENTS[10] = {
    { 101, "John Doe", 45, "Cardiovascular Hypertension", "Dr. Robert Smith", "Admitted" },
    { 102, "Emily Watson", 32, "Type 2 Diabetes Mellitus", "Dr. Sarah Jenkins", "Under Observation" },
    { 103, "Michael Chang", 58, "Acute Bronchitis", "Dr. Emily Davis", "Admitted" },
    { 104, "Sophia Martinez", 24, "Migraine & Neuropathy", "Dr. James Wilson", "Discharged" },
    { 105, "William Johnson", 67, "Osteoarthritis", "Dr. Amanda White", "Admitted" },
    { 106, "Olivia Taylor", 29, "Gastroenteritis", "Dr. Robert Smith", "Discharged" },
    { 107, "Daniel Anderson", 50, "Chronic Kidney Disease", "Dr. Lisa Taylor", "Admitted" },
    { 108, "Ava Thomas", 19, "Allergic Asthma", "Dr. Emily Davis", "Under Observation" },
    { 109, "David Brown", 41, "Fractured Fibula", "Dr. Amanda White", "Admitted" },
    { 110, "Isabella Garcia", 36, "Hyperthyroidism", "Dr. Sarah Jenkins", "Discharged" }
};

// Forward Declarations
void load_database(void);
void save_database(void);
void reset_seed_database(void);
int add_patient(const char *name, int age, const char *disease, const char *doctor, const char *status);
int update_patient(int id, const char *name, int age, const char *disease, const char *doctor, const char *status);
void print_patient_table(void);
void search_patients(const char *query);
void print_statistics(void);
void run_console_menu(void);
void run_web_server(void);

// Helper to escape string for JSON
static void json_escape(const char *src, char *dest, size_t max_len) {
    size_t j = 0;
    for (size_t i = 0; src[i] != '\0' && j + 2 < max_len; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dest[j++] = '\\';
            dest[j++] = src[i];
        } else if (src[i] == '\n') {
            dest[j++] = '\\';
            dest[j++] = 'n';
        } else if (src[i] == '\r') {
            dest[j++] = '\\';
            dest[j++] = 'r';
        } else {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
}

// Generate JSON string from database
static void serialize_to_json(char *buffer, size_t max_size) {
    size_t offset = 0;
    offset += snprintf(buffer + offset, max_size - offset, "[\n");

    for (int i = 0; i < patient_count; i++) {
        char esc_name[128], esc_disease[128], esc_doctor[128], esc_status[64];
        json_escape(patients[i].name, esc_name, sizeof(esc_name));
        json_escape(patients[i].disease, esc_disease, sizeof(esc_disease));
        json_escape(patients[i].doctor, esc_doctor, sizeof(esc_doctor));
        json_escape(patients[i].status, esc_status, sizeof(esc_status));

        int written = snprintf(buffer + offset, max_size - offset,
            "  {\n"
            "    \"id\": %d,\n"
            "    \"name\": \"%s\",\n"
            "    \"age\": %d,\n"
            "    \"disease\": \"%s\",\n"
            "    \"doctor\": \"%s\",\n"
            "    \"status\": \"%s\"\n"
            "  }%s\n",
            patients[i].id,
            esc_name,
            patients[i].age,
            esc_disease,
            esc_doctor,
            esc_status,
            (i < patient_count - 1) ? "," : "");

        if (written < 0 || (size_t)written >= max_size - offset) break;
        offset += written;
    }

    snprintf(buffer + offset, max_size - offset, "]\n");
}

// Save Database to Binary File
void save_database(void) {
    FILE *fp = fopen(DB_FILE, "wb");
    if (fp) {
        fwrite(&patient_count, sizeof(int), 1, fp);
        if (patient_count > 0) {
            fwrite(patients, sizeof(Patient), patient_count, fp);
        }
        fclose(fp);
    }
}

// Reset Database with 10 Default Demo Records
void reset_seed_database(void) {
    patient_count = 10;
    for (int i = 0; i < 10; i++) {
        patients[i] = SEED_PATIENTS[i];
    }
    save_database();
    printf("[DATABASE] Initialized with 10 default patient records.\n");
}

// Load Database from disk
void load_database(void) {
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp) {
        reset_seed_database();
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, fp) == 1) {
        if (count >= 0 && count <= MAX_PATIENTS) {
            patient_count = count;
            if (patient_count > 0) {
                fread(patients, sizeof(Patient), patient_count, fp);
            }
        }
    }
    fclose(fp);

    if (patient_count == 0) {
        reset_seed_database();
    }
}

// Add Patient
int add_patient(const char *name, int age, const char *disease, const char *doctor, const char *status) {
    if (patient_count >= MAX_PATIENTS) return -1;

    int new_id = 101;
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id >= new_id) {
            new_id = patients[i].id + 1;
        }
    }

    Patient p;
    p.id = new_id;
    strncpy(p.name, name && *name ? name : "Unknown", sizeof(p.name) - 1);
    p.name[sizeof(p.name) - 1] = '\0';
    p.age = age > 0 ? age : 1;
    strncpy(p.disease, disease && *disease ? disease : "General Checkup", sizeof(p.disease) - 1);
    p.disease[sizeof(p.disease) - 1] = '\0';
    strncpy(p.doctor, doctor && *doctor ? doctor : "Duty Doctor", sizeof(p.doctor) - 1);
    p.doctor[sizeof(p.doctor) - 1] = '\0';
    strncpy(p.status, status && *status ? status : "Admitted", sizeof(p.status) - 1);
    p.status[sizeof(p.status) - 1] = '\0';

    patients[patient_count++] = p;
    save_database();
    return new_id;
}

// Update Patient
int update_patient(int id, const char *name, int age, const char *disease, const char *doctor, const char *status) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == id) {
            if (name && strlen(name) > 0) {
                strncpy(patients[i].name, name, sizeof(patients[i].name) - 1);
                patients[i].name[sizeof(patients[i].name) - 1] = '\0';
            }
            if (age > 0) patients[i].age = age;
            if (disease && strlen(disease) > 0) {
                strncpy(patients[i].disease, disease, sizeof(patients[i].disease) - 1);
                patients[i].disease[sizeof(patients[i].disease) - 1] = '\0';
            }
            if (doctor && strlen(doctor) > 0) {
                strncpy(patients[i].doctor, doctor, sizeof(patients[i].doctor) - 1);
                patients[i].doctor[sizeof(patients[i].doctor) - 1] = '\0';
            }
            if (status && strlen(status) > 0) {
                strncpy(patients[i].status, status, sizeof(patients[i].status) - 1);
                patients[i].status[sizeof(patients[i].status) - 1] = '\0';
            }
            save_database();
            return 1;
        }
    }
    return 0;
}

// Display Patient Table in Console
void print_patient_table(void) {
    printf("\n+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");
    printf("| %-3s | %-20s | %-3s | %-27s | %-19s | %-17s |\n", "ID", "Patient Name", "Age", "Disease / Diagnosis", "Attending Doctor", "Status");
    printf("+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");

    if (patient_count == 0) {
        printf("| %-101s |\n", "No patient records found in database.");
    } else {
        for (int i = 0; i < patient_count; i++) {
            printf("| %-3d | %-20.20s | %-3d | %-27.27s | %-19.19s | %-17.17s |\n",
                   patients[i].id,
                   patients[i].name,
                   patients[i].age,
                   patients[i].disease,
                   patients[i].doctor,
                   patients[i].status);
        }
    }
    printf("+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");
    printf("Total Records: %d\n\n", patient_count);
}

// Case-insensitive substring check
static int str_contains_nocase(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    size_t needle_len = strlen(needle);
    if (needle_len == 0) return 1;

    for (; *haystack; haystack++) {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
            size_t i;
            for (i = 1; i < needle_len; i++) {
                if (!haystack[i] || tolower((unsigned char)haystack[i]) != tolower((unsigned char)needle[i]))
                    break;
            }
            if (i == needle_len) return 1;
        }
    }
    return 0;
}

// Search Patients
void search_patients(const char *query) {
    printf("\n[SEARCH RESULTS for '%s']\n", query);
    printf("+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");
    printf("| %-3s | %-20s | %-3s | %-27s | %-19s | %-17s |\n", "ID", "Patient Name", "Age", "Disease / Diagnosis", "Attending Doctor", "Status");
    printf("+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");

    int matches = 0;
    for (int i = 0; i < patient_count; i++) {
        char id_str[16];
        snprintf(id_str, sizeof(id_str), "%d", patients[i].id);

        if (str_contains_nocase(patients[i].name, query) ||
            str_contains_nocase(patients[i].disease, query) ||
            str_contains_nocase(patients[i].doctor, query) ||
            str_contains_nocase(patients[i].status, query) ||
            str_contains_nocase(id_str, query)) {
            printf("| %-3d | %-20.20s | %-3d | %-27.27s | %-19.19s | %-17.17s |\n",
                   patients[i].id,
                   patients[i].name,
                   patients[i].age,
                   patients[i].disease,
                   patients[i].doctor,
                   patients[i].status);
            matches++;
        }
    }
    printf("+-----+----------------------+-----+-----------------------------+---------------------+-------------------+\n");
    printf("Matches Found: %d\n\n", matches);
}

// Statistics Summary
void print_statistics(void) {
    int admitted = 0, observation = 0, discharged = 0;
    for (int i = 0; i < patient_count; i++) {
        if (strcmp(patients[i].status, "Admitted") == 0) admitted++;
        else if (strcmp(patients[i].status, "Under Observation") == 0) observation++;
        else if (strcmp(patients[i].status, "Discharged") == 0) discharged++;
    }

    printf("\n========================================\n");
    printf("   HOSPITAL RECORD SYSTEM STATISTICS    \n");
    printf("========================================\n");
    printf(" Total Registered Patients : %d\n", patient_count);
    printf(" Currently Admitted        : %d\n", admitted);
    printf(" Under Observation         : %d\n", observation);
    printf(" Discharged Patients       : %d\n", discharged);
    printf(" Database File             : %s (Binary Persistent)\n", DB_FILE);
    printf("========================================\n\n");
}

// Simple JSON Parser Helper
static void extract_json_value(const char *json, const char *key, char *out, size_t max_len) {
    out[0] = '\0';
    char search_pattern[64];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\"", key);

    const char *pos = strstr(json, search_pattern);
    if (!pos) return;

    pos += strlen(search_pattern);
    while (*pos == ' ' || *pos == ':' || *pos == '\t' || *pos == '\r' || *pos == '\n') pos++;

    if (*pos == '"') {
        pos++;
        size_t idx = 0;
        while (*pos && *pos != '"' && idx + 1 < max_len) {
            if (*pos == '\\' && *(pos + 1)) pos++;
            out[idx++] = *pos++;
        }
        out[idx] = '\0';
    } else {
        size_t idx = 0;
        while (*pos && *pos != ',' && *pos != '}' && *pos != ' ' && *pos != '\r' && *pos != '\n' && idx + 1 < max_len) {
            out[idx++] = *pos++;
        }
        out[idx] = '\0';
    }
}

// Send complete HTTP Response
static void send_http_response(SOCKET client, int status_code, const char *status_text, const char *content_type, const char *body, size_t body_len) {
    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s; charset=UTF-8\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n\r\n",
        status_code, status_text, content_type, body_len);

    send(client, header, header_len, 0);
    if (body_len > 0 && body) {
        send(client, body, (int)body_len, 0);
    }
}

// Read and serve a static file
static void serve_static_file(SOCKET client, const char *filename, const char *content_type) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        const char *not_found = "404 Not Found";
        send_http_response(client, 404, "Not Found", "text/plain", not_found, strlen(not_found));
        return;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = (char *)malloc(fsize + 1);
    if (!buf) {
        fclose(fp);
        const char *err = "500 Server Memory Error";
        send_http_response(client, 500, "Internal Server Error", "text/plain", err, strlen(err));
        return;
    }

    fread(buf, 1, fsize, fp);
    fclose(fp);

    send_http_response(client, 200, "OK", content_type, buf, fsize);
    free(buf);
}

// Integrated Winsock Web & REST Server
void run_web_server(void) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    printf("\n[WEB SERVER] Initializing Windows Sockets...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ERROR] WSAStartup failed. Error: %d\n", WSAGetLastError());
        return;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("[ERROR] Bind failed on port %d. Error: %d\n", PORT, WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("[ERROR] Listen failed. Error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    printf("=================================================================\n");
    printf("  HOSPITAL MANAGEMENT SYSTEM - C BACKEND SERVER RUNNING!\n");
    printf("  Web Dashboard: http://localhost:%d\n", PORT);
    printf("  REST Endpoints:\n");
    printf("    - GET  /api/patients\n");
    printf("    - POST /api/patients\n");
    printf("    - GET  /api/stats\n");
    printf("  Press Ctrl+C to stop the server.\n");
    printf("=================================================================\n\n");

    char *recv_buffer = (char *)malloc(BUFFER_SIZE);
    char *resp_buffer = (char *)malloc(BUFFER_SIZE * 2);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) continue;

        int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            recv_buffer[bytes_received] = '\0';

            char method[16] = {0}, url[256] = {0};
            sscanf(recv_buffer, "%15s %255s", method, url);

            printf("[HTTP] %s %s\n", method, url);

            // Handle OPTIONS preflight
            if (strcmp(method, "OPTIONS") == 0) {
                send_http_response(client_socket, 204, "No Content", "text/plain", "", 0);
            }
            // API: GET Patients
            else if (strcmp(method, "GET") == 0 && strncmp(url, "/api/patients", 13) == 0) {
                serialize_to_json(resp_buffer, BUFFER_SIZE * 2);
                send_http_response(client_socket, 200, "OK", "application/json", resp_buffer, strlen(resp_buffer));
            }
            // API: GET Stats
            else if (strcmp(method, "GET") == 0 && strncmp(url, "/api/stats", 10) == 0) {
                int admitted = 0, observation = 0, discharged = 0;
                for (int i = 0; i < patient_count; i++) {
                    if (strcmp(patients[i].status, "Admitted") == 0) admitted++;
                    else if (strcmp(patients[i].status, "Under Observation") == 0) observation++;
                    else if (strcmp(patients[i].status, "Discharged") == 0) discharged++;
                }
                snprintf(resp_buffer, BUFFER_SIZE,
                    "{\"total\": %d, \"admitted\": %d, \"observation\": %d, \"discharged\": %d}\n",
                    patient_count, admitted, observation, discharged);
                send_http_response(client_socket, 200, "OK", "application/json", resp_buffer, strlen(resp_buffer));
            }
            // API: POST Patient
            else if (strcmp(method, "POST") == 0 && strncmp(url, "/api/patients", 13) == 0) {
                char *body = strstr(recv_buffer, "\r\n\r\n");
                if (body) body += 4;
                else body = "";

                char name[64] = {0}, age_str[16] = {0}, disease[64] = {0}, doctor[64] = {0}, status[32] = {0};
                extract_json_value(body, "name", name, sizeof(name));
                extract_json_value(body, "age", age_str, sizeof(age_str));
                extract_json_value(body, "disease", disease, sizeof(disease));
                extract_json_value(body, "doctor", doctor, sizeof(doctor));
                extract_json_value(body, "status", status, sizeof(status));

                int age = atoi(age_str);
                int new_id = add_patient(name, age, disease, doctor, status);

                snprintf(resp_buffer, BUFFER_SIZE,
                    "{\"status\": \"success\", \"id\": %d, \"message\": \"Patient registered successfully\"}\n",
                    new_id);
                send_http_response(client_socket, 201, "Created", "application/json", resp_buffer, strlen(resp_buffer));
            }
            // Serve Web UI: / or /index.html or /index.HTML
            else if (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0 || strcmp(url, "/index.HTML") == 0) {
                serve_static_file(client_socket, "index.HTML", "text/html");
            }
            // Serve CSS
            else if (strcmp(url, "/H2.css") == 0 || strcmp(url, "/H2.CSS") == 0) {
                serve_static_file(client_socket, "H2.css", "text/css");
            }
            // Serve JS
            else if (strcmp(url, "/H3.js") == 0) {
                serve_static_file(client_socket, "H3.js", "application/javascript");
            }
            else if (strcmp(url, "/database.sql") == 0) {
                serve_static_file(client_socket, "database.sql", "text/plain");
            }
            else {
                const char *not_found = "404 Route Not Found";
                send_http_response(client_socket, 404, "Not Found", "text/plain", not_found, strlen(not_found));
            }
        }
        closesocket(client_socket);
    }

    free(recv_buffer);
    free(resp_buffer);
    closesocket(server_socket);
    WSACleanup();
}

// Interactive CLI Console Menu
void run_console_menu(void) {
    int choice;
    char buffer[256];

    while (1) {
        printf("\n================================================================\n");
        printf("         HOSPITAL MANAGEMENT SYSTEM - C BACKEND CONSOLE         \n");
        printf("================================================================\n");
        printf(" [1] View All Patient Records\n");
        printf(" [2] Register New Patient\n");
        printf(" [3] Search Patient (by Name, ID, Disease, Doctor)\n");
        printf(" [4] Update Patient Record\n");
        printf(" [5] View Hospital Statistical Summary\n");
        printf(" [6] Reset Database to 10 Default Demo Records\n");
        printf(" [7] Start Integrated Web & REST API Server (Port 8080)\n");
        printf(" [0] Exit Application\n");
        printf("================================================================\n");
        printf("Enter your choice (0-7): ");

        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        choice = atoi(buffer);

        switch (choice) {
            case 1:
                print_patient_table();
                break;
            case 2: {
                char name[64], disease[64], doctor[64], status[32], age_str[16];
                printf("Enter Patient Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\r\n")] = '\0';

                printf("Enter Age: ");
                fgets(age_str, sizeof(age_str), stdin);
                int age = atoi(age_str);

                printf("Enter Disease / Diagnosis: ");
                fgets(disease, sizeof(disease), stdin);
                disease[strcspn(disease, "\r\n")] = '\0';

                printf("Enter Attending Doctor: ");
                fgets(doctor, sizeof(doctor), stdin);
                doctor[strcspn(doctor, "\r\n")] = '\0';

                printf("Enter Status (1: Admitted, 2: Under Observation, 3: Discharged): ");
                fgets(status, sizeof(status), stdin);
                int s_opt = atoi(status);
                const char *st = (s_opt == 2) ? "Under Observation" : ((s_opt == 3) ? "Discharged" : "Admitted");

                int new_id = add_patient(name, age, disease, doctor, st);
                printf("\n[SUCCESS] Patient successfully registered with ID: #%d\n", new_id);
                break;
            }
            case 3: {
                char query[64];
                printf("Enter search term (Name, ID, Doctor, or Disease): ");
                fgets(query, sizeof(query), stdin);
                query[strcspn(query, "\r\n")] = '\0';
                search_patients(query);
                break;
            }
            case 4: {
                char id_str[16], name[64], disease[64], doctor[64], status[32], age_str[16];
                printf("Enter Patient ID to update: ");
                fgets(id_str, sizeof(id_str), stdin);
                int id = atoi(id_str);

                printf("Enter New Name (leave blank to keep current): ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\r\n")] = '\0';

                printf("Enter New Age (0 to keep current): ");
                fgets(age_str, sizeof(age_str), stdin);
                int age = atoi(age_str);

                printf("Enter New Disease (leave blank to keep current): ");
                fgets(disease, sizeof(disease), stdin);
                disease[strcspn(disease, "\r\n")] = '\0';

                printf("Enter New Doctor (leave blank to keep current): ");
                fgets(doctor, sizeof(doctor), stdin);
                doctor[strcspn(doctor, "\r\n")] = '\0';

                printf("Enter New Status (leave blank to keep current): ");
                fgets(status, sizeof(status), stdin);
                status[strcspn(status, "\r\n")] = '\0';

                if (update_patient(id, name, age, disease, doctor, status)) {
                    printf("\n[SUCCESS] Patient record #%d successfully updated.\n", id);
                } else {
                    printf("\n[ERROR] Patient ID #%d not found.\n", id);
                }
                break;
            }
            case 5:
                print_statistics();
                break;
            case 6:
                reset_seed_database();
                break;
            case 7:
                run_web_server();
                break;
            case 0:
                printf("\nExiting Hospital Management System. Goodbye!\n");
                return;
            default:
                printf("\nInvalid choice. Please enter a number between 0 and 7.\n");
        }
    }
}

// Program Entry Point
int main(int argc, char *argv[]) {
    load_database();

    if (argc > 1) {
        if (strcmp(argv[1], "--server") == 0 || strcmp(argv[1], "-s") == 0) {
            run_web_server();
            return 0;
        } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage:\n");
            printf("  hospital_backend.exe          Run interactive CLI console\n");
            printf("  hospital_backend.exe --server Start integrated Web & REST API Server\n");
            return 0;
        }
    }

    run_console_menu();
    return 0;
}
