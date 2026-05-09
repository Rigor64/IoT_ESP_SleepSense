# WiFi Test - Procedura

Questo script testa la connessione WiFi dell'ESP32.

## ⚠️ Sicurezza

Il file `WiFi_test.ino` NON deve contenere credenziali reali e NON deve essere committato.
Per questo motivo è presente un template: `WiFi_test.ino.example`

## Come usare

1. Copia `WiFi_test.ino.example` in `WiFi_test.ino`:
   ```bash
   cp WiFi_test.ino.example WiFi_test.ino
   ```

2. Modifica `WiFi_test.ino` con le tue credenziali:
   ```cpp
   const char* ssid     = "TuoSSID";
   const char* password = "TuaPassword";
   ```

3. Carica lo sketch sull'ESP32

4. ⚠️ **IMPORTANTE**: Non committare `WiFi_test.ino` - è già in `.gitignore`

## Note

- Il file `.gitignore` esclude automaticamente `WiFi_test.ino` dal versionamento
- Mantieni sempre le credenziali fuori dalla repository pubblica
- Se accidentalmente commitjate credenziali, ruotatele immediatamente
