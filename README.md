# ESP32-GMP
ESP32 untuk mendeteksi gempa memakai MPU6050 dan Buzzer. Dah itu aja

Bisa digunakan memakai platform.io dan Arduino IDE. Tetapi saya memakai platform.io, sehingga saya menyarankan memakai platform.io.
Dari kode ini sangat masih jauh dari sempurna, saya berharap jadikan ini sebagai referensi ataupun anda bisa membuat yang lebih bagus daripada yang saya buat.
Terimakasih!

# 🌍 Detektor Gempa Bumi dengan ESP32 & MPU6050

Sistem pendeteksi gempa bumi secara real-time menggunakan mikrokontroler ESP32 dan sensor akselerometer MPU6050. Sistem ini memantau getaran secara terus-menerus dan membunyikan alarm buzzer ketika deviasi akselerasi melebihi batas yang ditentukan.

---

## 📋 Daftar Isi

- [Fitur](#fitur)
- [Kebutuhan Hardware](#kebutuhan-hardware)
- [Diagram Pengkabelan](#diagram-pengkabelan)
- [Kebutuhan Software](#kebutuhan-software)
- [Instalasi](#instalasi)
- [Konfigurasi](#konfigurasi)
- [Cara Kerja](#cara-kerja)
- [Output Serial Monitor](#output-serial-monitor)
- [Lisensi](#lisensi)

---

## ✨ Fitur

- Pembacaan akselerometer secara real-time via I2C (MPU6050)
- Kalibrasi sensor otomatis saat pertama dinyalakan
- Alarm buzzer dengan pola bunyi berulang (ON/OFF setiap 200ms)
- Serial monitor dengan status langsung: `[OK]` / `[WARN]` / `[ALARM]`
- Progress bar visual yang menunjukkan tingkat deviasi
- Alarm berhenti otomatis setelah 5 detik

---

## 🛠️ Kebutuhan Hardware

| Komponen | Jumlah |
|----------|--------|
| ESP32 Development Board | 1 |
| Modul Akselerometer/Giroskop MPU6050 | 1 |
| Buzzer Pasif/Aktif | 1 |
| Kabel Jumper | Secukupnya |
| Breadboard | 1 |

---

## 🔌 Diagram Pengkabelan

| Pin MPU6050 | Pin ESP32 |
|-------------|-----------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

| Pin Buzzer | Pin ESP32 |
|------------|-----------|
| (+) / Signal | GPIO 25 |
| (-) / GND | GND |

---

## 💻 Kebutuhan Software

- [PlatformIO](https://platformio.org/) atau [Arduino IDE](https://www.arduino.cc/en/software)
- Paket Board ESP32
- Library (sudah termasuk dalam ESP32 core):
  - `Arduino.h`
  - `Wire.h` (komunikasi I2C)

---

## 🚀 Instalasi

1. **Clone repositori ini:**
   ```bash
   git clone https://github.com/yourusername/earthquake-detector-esp32.git
   cd earthquake-detector-esp32
   ```

2. **Buka proyek:**
   - **PlatformIO:** Buka folder di VS Code dengan ekstensi PlatformIO
   - **Arduino IDE:** Buka `main.cpp` (ganti nama menjadi `.ino` jika diperlukan)

3. **Pilih board yang sesuai:**
   - Board: `ESP32 Dev Module`
   - Upload Speed: `115200`

4. **Upload ke ESP32:**
   ```bash
   # PlatformIO
   pio run --target upload
   ```

5. **Buka Serial Monitor pada baud rate `115200`** untuk melihat output.

---

## ⚙️ Konfigurasi

Anda dapat mengubah konstanta berikut di `main.cpp` untuk menyesuaikan sistem:

```cpp
#define BUZZER_PIN 25              // Pin GPIO untuk buzzer
#define EARTHQUAKE_THRESHOLD 3.0  // Batas deviasi dalam m/s² untuk memicu alarm
const unsigned long ALARM_DURATION = 5000; // Durasi alarm dalam milidetik
const int CALIBRATION_SAMPLES = 100;       // Jumlah sampel untuk kalibrasi
```

| Parameter | Nilai Default | Keterangan |
|-----------|---------------|------------|
| `EARTHQUAKE_THRESHOLD` | `3.0 m/s²` | Deviasi minimum untuk memicu alarm |
| `ALARM_DURATION` | `5000 ms` | Lama alarm berbunyi |
| `CALIBRATION_SAMPLES` | `100` | Jumlah sampel saat kalibrasi awal |

---

## ⚙️ Cara Kerja

```
[Startup]
   │
   ├─► Tes buzzer (3 kali bunyi)
   ├─► Inisialisasi MPU6050 via I2C
   └─► Kalibrasi sensor (100 sampel → akselerasi baseline)

[Loop Utama - setiap 50ms]
   │
   ├─► Baca akselerometer (sumbu X, Y, Z)
   ├─► Hitung magnitude: √(ax² + ay² + az²)
   ├─► Hitung deviasi: |magnitude - baseline|
   │
   ├─► deviasi > 3.0 m/s²?
   │       YA  → Aktifkan alarm buzzer selama 5 detik
   │       TIDAK → Lanjutkan pemantauan
   │
   └─► Tampilkan status ke Serial setiap 500ms
```

---

## 📟 Output Serial Monitor

```
Initializing MPU6050...
MPU6050 connected successfully!
Calibrating sensor...
JANGAN GERAKKAN SENSOR!
..........
Baseline acceleration: 9.82 m/s²
Calibration complete!

Accel: 9.83 m/s² | Dev: 0.01 m/s² | [OK]    [----------]
Accel: 9.84 m/s² | Dev: 0.02 m/s² | [OK]    [----------]

========================
!!! GEMPA TERDETEKSI !!!
========================
Magnitude: 13.45 m/s² | Deviation: 3.63 m/s²
X: 8.21  Y: 9.14  Z: 5.72

Accel: 13.45 m/s² | Dev: 3.63 m/s² | [ALARM] [#################---]
=== Alarm stopped ===
```

---

## 📄 Lisensi

Proyek ini dilisensikan di bawah Lisensi MIT. Lihat file [LICENSE](LICENSE) untuk detailnya.

---

## 🙏 Ucapan Terima Kasih

- Datasheet MPU6050 dan referensi protokol I2C
- ESP32 Arduino core oleh Espressif Systems
