-- ===================================================================
-- Hospital Management System - Comprehensive Database Schema (SQL)
-- Compatible with SQLite3, MySQL, and PostgreSQL
-- ===================================================================

-- 1. Departments Table
CREATE TABLE IF NOT EXISTS departments (
    dept_id INTEGER PRIMARY KEY,
    dept_name VARCHAR(100) NOT NULL,
    head_doctor VARCHAR(100),
    floor_number INT DEFAULT 1,
    phone_extension VARCHAR(20)
);

-- 2. Doctors Table
CREATE TABLE IF NOT EXISTS doctors (
    doctor_id INTEGER PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    specialization VARCHAR(100) NOT NULL,
    dept_id INT,
    contact_number VARCHAR(20),
    email VARCHAR(100),
    availability VARCHAR(50) DEFAULT 'On Duty',
    FOREIGN KEY (dept_id) REFERENCES departments(dept_id)
);

-- 3. Patients Table (Core Entity)
CREATE TABLE IF NOT EXISTS patients (
    patient_id INTEGER PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    age INT NOT NULL,
    gender VARCHAR(10) DEFAULT 'Unspecified',
    contact_number VARCHAR(20),
    address TEXT,
    disease VARCHAR(150) NOT NULL,
    doctor_id INT,
    doctor_name VARCHAR(100),
    status VARCHAR(30) DEFAULT 'Admitted', -- 'Admitted', 'Under Observation', 'Discharged'
    admission_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    discharge_date TIMESTAMP NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id)
);

-- 4. Rooms and Beds Table
CREATE TABLE IF NOT EXISTS rooms (
    room_id INTEGER PRIMARY KEY,
    room_number VARCHAR(20) NOT NULL UNIQUE,
    room_type VARCHAR(50) DEFAULT 'General Ward', -- 'ICU', 'Private', 'Semi-Private', 'General'
    bed_count INT DEFAULT 1,
    is_occupied BOOLEAN DEFAULT 0,
    current_patient_id INT NULL,
    FOREIGN KEY (current_patient_id) REFERENCES patients(patient_id)
);

-- 5. Appointments Table
CREATE TABLE IF NOT EXISTS appointments (
    appointment_id INTEGER PRIMARY KEY,
    patient_id INT NOT NULL,
    doctor_id INT NOT NULL,
    appointment_date DATETIME NOT NULL,
    reason TEXT,
    status VARCHAR(30) DEFAULT 'Scheduled', -- 'Scheduled', 'Completed', 'Cancelled'
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id),
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id)
);

-- 6. Medical Records / Diagnosis History Table
CREATE TABLE IF NOT EXISTS medical_records (
    record_id INTEGER PRIMARY KEY,
    patient_id INT NOT NULL,
    diagnosis TEXT NOT NULL,
    treatment_plan TEXT,
    blood_pressure VARCHAR(20),
    pulse_rate INT,
    temperature DECIMAL(4,1),
    recorded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id)
);

-- 7. Prescriptions Table
CREATE TABLE IF NOT EXISTS prescriptions (
    prescription_id INTEGER PRIMARY KEY,
    patient_id INT NOT NULL,
    doctor_id INT NOT NULL,
    medication_name VARCHAR(100) NOT NULL,
    dosage VARCHAR(50) NOT NULL,
    frequency VARCHAR(50) NOT NULL,
    duration_days INT DEFAULT 7,
    prescribed_date DATE DEFAULT CURRENT_DATE,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id),
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id)
);

-- 8. Billing & Invoices Table
CREATE TABLE IF NOT EXISTS billing (
    bill_id INTEGER PRIMARY KEY,
    patient_id INT NOT NULL,
    room_charges DECIMAL(10,2) DEFAULT 0.00,
    treatment_charges DECIMAL(10,2) DEFAULT 0.00,
    medicine_charges DECIMAL(10,2) DEFAULT 0.00,
    total_amount DECIMAL(10,2) NOT NULL,
    payment_status VARCHAR(30) DEFAULT 'Pending', -- 'Paid', 'Pending', 'Partially Paid'
    payment_method VARCHAR(50) DEFAULT 'Cash',
    billed_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id)
);

-- 9. Hospital Staff Table
CREATE TABLE IF NOT EXISTS staff (
    staff_id INTEGER PRIMARY KEY,
    full_name VARCHAR(100) NOT NULL,
    role VARCHAR(50) NOT NULL, -- 'Nurse', 'Pharmacist', 'Receptionist', 'Technician'
    dept_id INT,
    shift VARCHAR(20) DEFAULT 'Day', -- 'Day', 'Night', 'Rotational'
    FOREIGN KEY (dept_id) REFERENCES departments(dept_id)
);

-- 10. Emergency Contacts Table
CREATE TABLE IF NOT EXISTS emergency_contacts (
    contact_id INTEGER PRIMARY KEY,
    patient_id INT NOT NULL,
    contact_name VARCHAR(100) NOT NULL,
    relationship VARCHAR(50) NOT NULL,
    phone_number VARCHAR(20) NOT NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id)
);

-- ===================================================================
-- SEED DATA: 10 Departments
-- ===================================================================
INSERT INTO departments (dept_id, dept_name, head_doctor, floor_number, phone_extension) VALUES
(1, 'Cardiology', 'Dr. Robert Smith', 2, '201'),
(2, 'Endocrinology', 'Dr. Sarah Jenkins', 3, '305'),
(3, 'Pulmonology', 'Dr. Emily Davis', 2, '210'),
(4, 'Neurology', 'Dr. James Wilson', 4, '401'),
(5, 'Orthopedics', 'Dr. Amanda White', 1, '108'),
(6, 'Gastroenterology', 'Dr. Robert Smith', 3, '312'),
(7, 'Nephrology', 'Dr. Lisa Taylor', 4, '415'),
(8, 'Pediatrics', 'Dr. Emily Davis', 1, '102'),
(9, 'Trauma & Surgery', 'Dr. Amanda White', 1, '115'),
(10, 'General Medicine', 'Dr. Sarah Jenkins', 2, '204');

-- ===================================================================
-- SEED DATA: 10 Core Doctors
-- ===================================================================
INSERT INTO doctors (doctor_id, name, specialization, dept_id, contact_number, email) VALUES
(1, 'Dr. Robert Smith', 'Cardiologist', 1, '+1-555-0101', 'rsmith@hospital.org'),
(2, 'Dr. Sarah Jenkins', 'Endocrinologist', 2, '+1-555-0102', 'sjenkins@hospital.org'),
(3, 'Dr. Emily Davis', 'Pulmonologist', 3, '+1-555-0103', 'edavis@hospital.org'),
(4, 'Dr. James Wilson', 'Neurologist', 4, '+1-555-0104', 'jwilson@hospital.org'),
(5, 'Dr. Amanda White', 'Orthopedic Surgeon', 5, '+1-555-0105', 'awhite@hospital.org'),
(6, 'Dr. Lisa Taylor', 'Nephrologist', 7, '+1-555-0106', 'ltaylor@hospital.org'),
(7, 'Dr. David Miller', 'General Physician', 10, '+1-555-0107', 'dmiller@hospital.org'),
(8, 'Dr. Karen Walker', 'Pediatrician', 8, '+1-555-0108', 'kwalker@hospital.org'),
(9, 'Dr. Michael Chen', 'Anesthesiologist', 9, '+1-555-0109', 'mchen@hospital.org'),
(10, 'Dr. Rachel Green', 'Radiologist', 10, '+1-555-0110', 'rgreen@hospital.org');

-- ===================================================================
-- SEED DATA: 10 Realistic Patient Records
-- ===================================================================
INSERT INTO patients (patient_id, name, age, gender, disease, doctor_id, doctor_name, status) VALUES
(101, 'John Doe', 45, 'Male', 'Cardiovascular Hypertension', 1, 'Dr. Robert Smith', 'Admitted'),
(102, 'Emily Watson', 32, 'Female', 'Type 2 Diabetes Mellitus', 2, 'Dr. Sarah Jenkins', 'Under Observation'),
(103, 'Michael Chang', 58, 'Male', 'Acute Bronchitis', 3, 'Dr. Emily Davis', 'Admitted'),
(104, 'Sophia Martinez', 24, 'Female', 'Migraine & Neuropathy', 4, 'Dr. James Wilson', 'Discharged'),
(105, 'William Johnson', 67, 'Male', 'Osteoarthritis', 5, 'Dr. Amanda White', 'Admitted'),
(106, 'Olivia Taylor', 29, 'Female', 'Gastroenteritis', 1, 'Dr. Robert Smith', 'Discharged'),
(107, 'Daniel Anderson', 50, 'Male', 'Chronic Kidney Disease', 6, 'Dr. Lisa Taylor', 'Admitted'),
(108, 'Ava Thomas', 19, 'Female', 'Allergic Asthma', 3, 'Dr. Emily Davis', 'Under Observation'),
(109, 'David Brown', 41, 'Male', 'Fractured Fibula', 5, 'Dr. Amanda White', 'Admitted'),
(110, 'Isabella Garcia', 36, 'Female', 'Hyperthyroidism', 2, 'Dr. Sarah Jenkins', 'Discharged');
