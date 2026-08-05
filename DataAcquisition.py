import serial
import csv
import time
import re
import os

# Konfiguracja portu
PORT = 'COM5'  # Dla Windowsa zmień na np. 'COM3'
BAUDRATE = 115200

# Wzorzec Regex do wyłuskania samych wartości liczbowych (zarówno całkowitych, jak i zmiennoprzecinkowych)
# Oczekuje dokładnie takiego formatu: "TempUp: 26.7, TempDown: 20.0, HumUp: 75, HumDown: 60"
wzorzec = re.compile(r"TempUp:\s*([\d.]+),\s*TempDown:\s*([\d.]+),\s*HumUp:\s*([\d.]+),\s*HumDown:\s*([\d.]+),\s*PeltUp:\s*([\d.]+),\s*PeltDown:\s*([\d.]+)")

try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)
    ser.flushInput()
    print(f"Połączono z portem {PORT}")
except Exception as e:
    print(f"Błąd połączenia z portem {PORT}: {e}")
    exit()

# --- LOGIKA GENEROWANIA UNIKALNEJ NAZWY PLIKU ---
baza_nazwy = "SensorsData"  # wczytywać dane do folderu
rozszerzenie = ".csv"
licznik = 1

file_name = f"{baza_nazwy}_{licznik}{rozszerzenie}"

# Dopóki plik o sprawdzanej nazwie istnieje, zwiększaj licznik
while os.path.exists(file_name):
    licznik += 1
    file_name = f"{baza_nazwy}_{licznik}{rozszerzenie}"

print(f"Dane zostaną zapisane w nowym pliku: {file_name}")
# -----------------------------------------------

# Otwarcie pliku CSV
with open(file_name, mode='w', newline='') as plik_csv:
    writer = csv.writer(plik_csv)
    
    # Dodanie nagłówków tylko, jeśli zaczynamy nowy plik (lub po prostu dodajemy za każdym uruchomieniem)
    writer.writerow(["Czas", "TempUp_C", "TempDown_C", "HumUp_%", "HumDown_%", "PeltUp_C", "PeltDown_C"])
    
    print("Rozpoczęto zapis. Naciśnij Ctrl+C, aby zakończyć.")
    
    while True:
        try:
            # Odczyt z UART i usunięcie białych znaków (np. \r\n)
            linia = ser.readline().decode('utf-8').strip()
            
            if not linia:
                continue # Pomijamy puste linie, jeśli timeout minął
            
            # Próba dopasowania odebranego tekstu do wzorca
            dopasowanie = wzorzec.search(linia)
            
            if dopasowanie:
                # Wyciągnięcie konkretnych liczb z grup wyrażenia regularnego
                temp_up = dopasowanie.group(1)
                temp_down = dopasowanie.group(2)
                hum_up = dopasowanie.group(3)
                hum_down = dopasowanie.group(4)
                pelt_up = dopasowanie.group(5)
                pelt_down = dopasowanie.group(6)
                
                aktualny_czas = time.strftime("%H:%M:%S")
                
                # Zapis do pliku .csv
                writer.writerow([aktualny_czas, temp_up, temp_down, hum_up, hum_down, pelt_up, pelt_down])
                
                # Wypisanie sformatowanych danych w konsoli dla podglądu
                print(f"[{aktualny_czas}] Zapisano: T_up={temp_up}°C | T_down={temp_down}°C | H_up={hum_up}% | H_down={hum_down}% | P_up={pelt_up}°C | P_down={pelt_down}°C")
            else:
                # Logowanie niezidentyfikowanych ramek danych
                print(f"Pominięto nierozpoznaną linię: {linia}")
                
        except KeyboardInterrupt:
            print("\nZakończono zbieranie danych. Plik został bezpiecznie zamknięty.")
            break
        except UnicodeDecodeError:
            print("Błąd dekodowania znaków - pominięto ramkę.")
        except Exception as e:
            print(f"Wystąpił nieoczekiwany błąd: {e}")
            break