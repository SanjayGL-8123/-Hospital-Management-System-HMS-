# 🏥 Hospital Management System (HMS)

A complete, production-ready **Hospital Management System** featuring a responsive frontend web interface, an embedded **C Backend Program** with Windows Sockets (Winsock), and a persistent **Database Engine** (`hospital.db` and `database.sql`).

---

## 📑 Table of Contents
1. [Project Overview](#-project-overview)
2. [Resolved Errors & Technical Fixes](#-resolved-errors--technical-fixes)
   - [Fix 1: `H2.css` and `H3.js` ERR_FILE_NOT_FOUND](#fix-1-h2css-and-h3js-err_file_not_found)
   - [Fix 2: Clang `'stdio.h' file not found` & Undeclared Identifiers](#fix-2-clang-stdioh-file-not-found--undeclared-identifiers)
   - [Fix 3: Removal of Delete Button & Unwanted Bloat](#fix-3-removal-of-delete-button--unwanted-bloat)
   - [Fix 4: Removal of Extra and Duplicate Files](#fix-4-removal-of-extra-and-duplicate-files)
3. [Project Directory Structure](#-project-directory-structure)
4. [The 10 Core Features](#-the-10-core-features)
5. [The 10 Database Schema Tables (`database.sql`)](#-the-10-database-schema-tables-databasesql)
6. [The 10 Seed Patient Records](#-the-10-seed-patient-records)
7. [Step-by-Step Build & Run Guide](#-step-by-step-build--run-guide)
8. [REST API Endpoints](#-rest-api-endpoints)

---

## 🌟 Project Overview

The **Hospital Management System (HMS)** provides a single unified interface for patient admission, vital record tracking, and hospital capacity monitoring.

* **Frontend:** Built with HTML5 ([index.HTML](file:///d:/small-project/all%20html/Hospital%20Management%20System/index.HTML)), CSS3 ([H2.css](file:///d:/small-project/all%20html/Hospital%20Management%20System/H2.css)), and JavaScript ([H3.js](file:///d:/small-project/all%20html/Hospital%20Management%20System/H3.js)).
* **Backend:** Native C program ([hospital_backend.c](file:///d:/small-project/all%20html/Hospital%20Management%20System/hospital_backend.c)) compiled to [hospital_backend.exe](file:///d:/small-project/all%20html/Hospital%20Management%20System/hospital_backend.exe) using MinGW GCC.
* **Database:** Binary file persistence ([hospital.db](file:///d:/small-project/all%20html/Hospital%20Management%20System/hospital.db)) and 10-table relational SQL schema ([database.sql](file:///d:/small-project/all%20html/Hospital%20Management%20System/database.sql)).

---

## 🛠️ Resolved Errors & Technical Fixes

### Fix 1: `H2.css` and `H3.js` ERR_FILE_NOT_FOUND
* **Problem:** Loading the web page generated browser console errors:
  - `H2.css:1 Failed to load resource: net::ERR_FILE_NOT_FOUND`
  - `H3.js:1 Failed to load resource: net::ERR_FILE_NOT_FOUND`
* **Root Cause:** `index.HTML` contained links to `H2.css` and `H3.js`, but only misnamed files were present in the directory.
* **Solution:** Created [H2.css](file:///d:/small-project/all%20html/Hospital%20Management%20System/H2.css) with a medical theme and [H3.js](file:///d:/small-project/all%20html/Hospital%20Management%20System/H3.js) with live form handling, search filter, and backend synchronization.

---

### Fix 2: Clang `'stdio.h' file not found` & Undeclared Identifiers
* **Problem:** In the editor, background clang language tools reported:
  - `'stdio.h' file not found (code: pp_file_not_found)`
  - `Call to undeclared library function 'snprintf'`
  - `Use of undeclared identifier 'FILE'`
  - `Too many errors emitted, stopping now`
* **Root Cause:** Clang's default include path on Windows was not pointed to the MinGW-w64 include paths.
* **Solution:**
  1. Configured [compile_flags.txt](file:///d:/small-project/all%20html/Hospital%20Management%20System/compile_flags.txt) and [.clangd](file:///d:/small-project/all%20html/Hospital%20Management%20System/.clangd) to target `x86_64-w64-mingw32` and include `C:/mingw64/x86_64-w64-mingw32/include`.
  2. Created [.vscode/c_cpp_properties.json](file:///d:/small-project/all%20html/Hospital%20Management%20System/.vscode/c_cpp_properties.json) and [.vscode/settings.json](file:///d:/small-project/all%20html/Hospital%20Management%20System/.vscode/settings.json) with compiler path `C:/mingw64/bin/gcc.exe`.
  3. Added `#if defined(__has_include)` guards with standard library declarations directly into [hospital_backend.c](file:///d:/small-project/all%20html/Hospital%20Management%20System/hospital_backend.c) so editor linters never produce fatal header errors.

---

### Fix 3: Removal of Delete Button & Unwanted Bloat
* **Action:**
  - Removed `<th>Action</th>` and the `Delete` button from the table in [index.HTML](file:///d:/small-project/all%20html/Hospital%20Management%20System/index.HTML) and [H3.js](file:///d:/small-project/all%20html/Hospital%20Management%20System/H3.js).
  - The table columns are now clean: **Record ID, Patient Name, Age, Disease, Attending Doctor, Status**.
  - Removed delete endpoints and dead code from [hospital_backend.c](file:///d:/small-project/all%20html/Hospital%20Management%20System/hospital_backend.c).

---

### Fix 4: Removal of Extra and Duplicate Files
* **Action:** Removed redundant files (`H1.HTML`, `style.CSS`, `script.js`, `database.json`, `build.bat`, `run.bat`) to keep the repository clean and maintainable.

---

## 📁 Project Directory Structure

```text
Hospital Management System/
│
├── index.HTML               # Main User Interface (HTML5)
├── H2.css                   # Stylesheet (Responsive Medical Theme)
├── H3.js                    # Client-side Logic & API Sync
│
├── hospital_backend.c       # C Backend Source (CLI + Winsock HTTP REST Server)
├── hospital_backend.exe     # Compiled Native Executable (MinGW-W64 GCC)
├── hospital.db              # Persistent Binary Database File
├── database.sql             # Relational Database Schema (10 Tables + Seed Data)
│
├── compile_flags.txt        # Clang compiler & include paths configuration
├── .clangd                  # Clangd language server configuration
├── .vscode/                 # VS Code / IDE C/C++ settings
│   ├── c_cpp_properties.json
│   └── settings.json
└── README.md                # System documentation
```

---

## 🌟 The 10 Core Features

1. **Patient Intake Form:** Registration for patient full name, age, diagnosis, assigned physician, and admission status.
2. **Auto-Incrementing Record IDs:** Generates unique tracking numbers (`#101`, `#102`...).
3. **Instant Real-Time Search:** Search bar filtering by patient name, diagnosis, doctor, or ID.
4. **Admission Status Badges:** Visual status tags for `Admitted`, `Under Observation`, and `Discharged`.
5. **Hospital KPI Cards:** Header metrics tracking total registered patients and admission breakdown.
6. **Binary Storage Engine:** Fast C binary file operations saving directly into `hospital.db`.
7. **Dual-Mode Persistence:** Online synchronization with the C REST server, plus fallback to browser `localStorage`.
8. **Integrated Winsock REST Server:** Embedded C web server on port 8080 serving HTML, CSS, JS, and JSON APIs.
9. **Interactive Console CLI:** Command-line terminal menu for direct database management.
10. **Linter & Compiler Clean:** Zero warnings and zero errors under GCC (`-Wall -Wextra`) and Clang.

---

## 🗄️ The 10 Database Schema Tables (`database.sql`)

The [`database.sql`](file:///d:/small-project/all%20html/Hospital%20Management%20System/database.sql) file contains the relational database definition:

1. **`patients`** — Core records (ID, name, age, gender, contact, diagnosis, doctor, status, admission date).
2. **`doctors`** — Medical staff directory (ID, name, specialization, department ID, phone, email, duty status).
3. **`departments`** — Clinical divisions (Cardiology, Endocrinology, Pulmonology, Neurology, Orthopedics, etc.).
4. **`appointments`** — Scheduled patient-doctor consultations, dates, reasons, and status.
5. **`rooms`** — Inpatient bed inventory, ward type (ICU, Private, Semi-Private, General), and occupancy tracking.
6. **`medical_records`** — Vital signs log (blood pressure, pulse, temperature, treatment plan).
7. **`prescriptions`** — Drug orders, dosages, intake frequency, and duration.
8. **`billing`** — Itemized invoices for room, treatment, and medications with payment status.
9. **`staff`** — Support personnel records (nurses, pharmacists, technicians, shifts).
10. **`emergency_contacts`** — Next-of-kin contacts, relationships, and emergency numbers.

---

## 📋 The 10 Seed Patient Records

| Record ID | Patient Name | Age | Disease / Diagnosis | Attending Doctor | Status |
| :---: | :--- | :---: | :--- | :--- | :---: |
| **#101** | John Doe | 45 | Cardiovascular Hypertension | Dr. Robert Smith | `Admitted` |
| **#102** | Emily Watson | 32 | Type 2 Diabetes Mellitus | Dr. Sarah Jenkins | `Under Observation` |
| **#103** | Michael Chang | 58 | Acute Bronchitis | Dr. Emily Davis | `Admitted` |
| **#104** | Sophia Martinez | 24 | Migraine & Neuropathy | Dr. James Wilson | `Discharged` |
| **#105** | William Johnson | 67 | Osteoarthritis | Dr. Amanda White | `Admitted` |
| **#106** | Olivia Taylor | 29 | Gastroenteritis | Dr. Robert Smith | `Discharged` |
| **#107** | Daniel Anderson | 50 | Chronic Kidney Disease | Dr. Lisa Taylor | `Admitted` |
| **#108** | Ava Thomas | 19 | Allergic Asthma | Dr. Emily Davis | `Under Observation` |
| **#109** | David Brown | 41 | Fractured Fibula | Dr. Amanda White | `Admitted` |
| **#110** | Isabella Garcia | 36 | Hyperthyroidism | Dr. Sarah Jenkins | `Discharged` |

---

## ⚡ Step-by-Step Build & Run Guide

### Option 1: Full-Stack Web + C REST Server Mode (Recommended)

1. **Compile the C Backend:**
   ```powershell
   gcc -Wall -Wextra hospital_backend.c -o hospital_backend.exe -lws2_32
   ```

2. **Launch the Server:**
   ```powershell
   .\hospital_backend.exe --server
   ```
   *The server starts listening at **[http://localhost:8080](http://localhost:8080)**.*

3. **Open the Portal:**
   Visit **[http://localhost:8080](http://localhost:8080)** in your browser. The connection badge will confirm:
   ```text
   ● C Backend: Online (Port 8080)
   ```

---

### Option 2: Interactive Terminal Console (CLI Mode)

Run the backend executable without arguments:
```powershell
.\hospital_backend.exe
```

Console Menu:
```text
================================================================
         HOSPITAL MANAGEMENT SYSTEM - C BACKEND CONSOLE         
================================================================
 [1] View All Patient Records
 [2] Register New Patient
 [3] Search Patient (by Name, ID, Disease, Doctor)
 [4] Update Patient Record
 [5] View Hospital Statistical Summary
 [6] Reset Database to 10 Default Demo Records
 [7] Start Integrated Web & REST API Server (Port 8080)
 [0] Exit Application
================================================================
```

---

### Option 3: Standalone Browser Mode

Open [index.HTML](file:///d:/small-project/all%20html/Hospital%20Management%20System/index.HTML) directly in any web browser. The app runs smoothly in offline mode using `localStorage`.

---

## 🌐 REST API Endpoints

| Method | Endpoint | Description | Sample Output |
| :---: | :--- | :--- | :--- |
| **`GET`** | `/api/patients` | Returns JSON array of all patients | `[ { "id": 101, "name": "John Doe", ... } ]` |
| **`POST`** | `/api/patients` | Registers new patient into `hospital.db` | `{"status": "success", "id": 111}` |
| **`GET`** | `/api/stats` | Returns aggregate counts | `{"total": 10, "admitted": 5, "observation": 2, "discharged": 3}` |

---

*Hospital Management System • Version 2.0 • Powered by HTML5, CSS3, JavaScript, C, and Winsock.*
#   - H o s p i t a l - M a n a g e m e n t - S y s t e m - H M S -  
 