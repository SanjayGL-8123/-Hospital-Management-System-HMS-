// Hospital Management System - Front-end Controller (H3.js)

const DEFAULT_PATIENTS = [
    { id: 101, name: "John Doe", age: 45, disease: "Cardiovascular Hypertension", doctor: "Dr. Robert Smith", status: "Admitted" },
    { id: 102, name: "Emily Watson", age: 32, disease: "Type 2 Diabetes Mellitus", doctor: "Dr. Sarah Jenkins", status: "Under Observation" },
    { id: 103, name: "Michael Chang", age: 58, disease: "Acute Bronchitis", doctor: "Dr. Emily Davis", status: "Admitted" },
    { id: 104, name: "Sophia Martinez", age: 24, disease: "Migraine & Neuropathy", doctor: "Dr. James Wilson", status: "Discharged" },
    { id: 105, name: "William Johnson", age: 67, disease: "Osteoarthritis", doctor: "Dr. Amanda White", status: "Admitted" },
    { id: 106, name: "Olivia Taylor", age: 29, disease: "Gastroenteritis", doctor: "Dr. Robert Smith", status: "Discharged" },
    { id: 107, name: "Daniel Anderson", age: 50, disease: "Chronic Kidney Disease", doctor: "Dr. Lisa Taylor", status: "Admitted" },
    { id: 108, name: "Ava Thomas", age: 19, disease: "Allergic Asthma", doctor: "Dr. Emily Davis", status: "Under Observation" },
    { id: 109, name: "David Brown", age: 41, disease: "Fractured Fibula", doctor: "Dr. Amanda White", status: "Admitted" },
    { id: 110, name: "Isabella Garcia", age: 36, disease: "Hyperthyroidism", doctor: "Dr. Sarah Jenkins", status: "Discharged" }
];

let patients = [];
let backendConnected = false;
const API_BASE = window.location.origin.startsWith("http") ? "" : "http://localhost:8080";

// Initialize application
document.addEventListener("DOMContentLoaded", () => {
    initApp();
    setupEventListeners();
});

async function initApp() {
    await checkBackendConnection();
    if (!backendConnected) {
        loadFromLocalStorage();
    }
    renderPatients();
}

function setupEventListeners() {
    const form = document.getElementById("patientForm");
    if (form) {
        form.addEventListener("submit", handleAddPatient);
    }

    const searchInput = document.getElementById("searchInput");
    if (searchInput) {
        searchInput.addEventListener("input", (e) => {
            renderPatients(e.target.value.trim().toLowerCase());
        });
    }
}

// Check if C Backend REST API is reachable
async function checkBackendConnection() {
    const statusEl = document.getElementById("serverStatus");
    try {
        const controller = new AbortController();
        const timeoutId = setTimeout(() => controller.abort(), 1200);
        
        const res = await fetch(`${API_BASE}/api/patients`, { signal: controller.signal });
        clearTimeout(timeoutId);

        if (res.ok) {
            const data = await res.json();
            patients = Array.isArray(data) && data.length > 0 ? data : DEFAULT_PATIENTS;
            backendConnected = true;
            if (statusEl) {
                statusEl.className = "server-status online";
                statusEl.innerHTML = `<span class="status-dot"></span> C Backend: Online (Port 8080)`;
            }
            return;
        }
    } catch (err) {
        // Backend not running; fallback gracefully
    }

    backendConnected = false;
    if (statusEl) {
        statusEl.className = "server-status";
        statusEl.innerHTML = `<span class="status-dot"></span> Standalone / Offline Storage`;
    }
}

function loadFromLocalStorage() {
    const stored = localStorage.getItem("hms_patients");
    if (stored) {
        try {
            patients = JSON.parse(stored);
        } catch (e) {
            patients = [...DEFAULT_PATIENTS];
        }
    } else {
        patients = [...DEFAULT_PATIENTS];
        saveToLocalStorage();
    }
}

function saveToLocalStorage() {
    localStorage.setItem("hms_patients", JSON.stringify(patients));
}

async function handleAddPatient(e) {
    e.preventDefault();

    const nameInput = document.getElementById("name");
    const ageInput = document.getElementById("age");
    const diseaseInput = document.getElementById("disease");
    const doctorInput = document.getElementById("doctor");
    const statusSelect = document.getElementById("status");

    const newPatient = {
        id: Date.now() % 100000,
        name: nameInput.value.trim(),
        age: parseInt(ageInput.value, 10),
        disease: diseaseInput.value.trim(),
        doctor: doctorInput.value.trim(),
        status: statusSelect ? statusSelect.value : "Admitted"
    };

    if (backendConnected) {
        try {
            const res = await fetch(`${API_BASE}/api/patients`, {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(newPatient)
            });
            if (res.ok) {
                const saved = await res.json();
                if (saved && saved.id) newPatient.id = saved.id;
            }
        } catch (err) {
            console.warn("Could not sync with C backend, saving locally:", err);
        }
    }

    patients.unshift(newPatient);
    saveToLocalStorage();
    renderPatients();

    document.getElementById("patientForm").reset();
}

function renderPatients(query = "") {
    const tbody = document.getElementById("patientTable");
    if (!tbody) return;

    tbody.innerHTML = "";

    const filtered = patients.filter(p => {
        if (!query) return true;
        return (
            (p.name && p.name.toLowerCase().includes(query)) ||
            (p.disease && p.disease.toLowerCase().includes(query)) ||
            (p.doctor && p.doctor.toLowerCase().includes(query)) ||
            (p.status && p.status.toLowerCase().includes(query)) ||
            (p.id && String(p.id).includes(query))
        );
    });

    if (filtered.length === 0) {
        tbody.innerHTML = `<tr><td colspan="6" class="empty-state">No matching patient records found.</td></tr>`;
    } else {
        filtered.forEach(p => {
            const row = tbody.insertRow();
            
            const badgeClass = p.status === "Discharged" 
                ? "badge-discharged" 
                : (p.status === "Under Observation" ? "badge-observation" : "badge-admitted");

            row.innerHTML = `
                <td><strong>#${p.id}</strong></td>
                <td>${escapeHtml(p.name)}</td>
                <td>${p.age}</td>
                <td>${escapeHtml(p.disease)}</td>
                <td>${escapeHtml(p.doctor)}</td>
                <td><span class="badge ${badgeClass}">${escapeHtml(p.status || "Admitted")}</span></td>
            `;
        });
    }

    updateStats();
}

function updateStats() {
    const totalEl = document.getElementById("statTotal");
    const admittedEl = document.getElementById("statAdmitted");
    const observationEl = document.getElementById("statObservation");
    const dischargedEl = document.getElementById("statDischarged");

    if (totalEl) totalEl.textContent = patients.length;
    if (admittedEl) admittedEl.textContent = patients.filter(p => p.status === "Admitted").length;
    if (observationEl) observationEl.textContent = patients.filter(p => p.status === "Under Observation").length;
    if (dischargedEl) dischargedEl.textContent = patients.filter(p => p.status === "Discharged").length;
}

function escapeHtml(str) {
    if (!str) return "";
    return String(str)
        .replace(/&/g, "&amp;")
        .replace(/</g, "&lt;")
        .replace(/>/g, "&gt;")
        .replace(/"/g, "&quot;")
        .replace(/'/g, "&#039;");
}
