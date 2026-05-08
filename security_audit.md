# Security Checklist - IoT_ESP_SleepSense

## Situazione rilevata
È stato individuato un forte indizio di esposizione di segreti nella cronologia Git della repository.

Commit rilevante:
- `1914d54bcaf6b738e5c9b9c683a36e721cff80ec`
- Messaggio: `Add secret.h file / Remove secrets in SleepSense.ino`

Link:
- https://github.com/Rigor64/IoT_ESP_SleepSense/commit/1914d54bcaf6b738e5c9b9c683a36e721cff80ec

Questo suggerisce che:
1. credenziali o segreti erano probabilmente presenti in `SleepSense.ino`
2. in seguito sono stati spostati in `secret.h`

---

## Cosa devi fare subito

### 1. Verificare se `secret.h` è presente nella repository
Controlla se il file `secret.h` è:
- visibile nella repo pubblica
- stato committato in passato
- incluso per errore in branch diversi

Se `secret.h` è su GitHub, i segreti sono da considerare compromessi.

### 2. Controllare i file più sensibili
Verifica prima questi file:
- `SleepSense.ino`
- `secret.h`
- `.gitignore`
- eventuali file `.txt`
- notebook `.ipynb`
- file di configurazione
- dump, export, backup o file CSV con dati sensibili

Cerca stringhe come:
- `password`
- `passwd`
- `pwd`
- `ssid`
- `token`
- `api_key`
- `apikey`
- `secret`
- `client_secret`
- `mqtt`
- `wifi`
- `telegram`
- `bot token`
- `firebase`
- `aws_secret_access_key`
- `connectionString`

### 3. Ruotare tutte le credenziali
Se una credenziale è stata anche solo potenzialmente pubblicata, deve essere sostituita.

Ruota subito:
- password Wi‑Fi
- token bot Telegram
- API key
- credenziali MQTT
- credenziali cloud o database
- eventuali chiavi private

### 4. Non limitarti a rimuovere i segreti dal file corrente
Rimuovere i segreti dall'ultima versione non basta se sono già stati pubblicati nella cronologia Git.

Devi verificare:
- commit precedenti
- branch
- tag
- fork eventuali

### 5. Escludere `secret.h` dal versionamento
Aggiungi `secret.h` al file `.gitignore` se non è già escluso.

Esempio:
```gitignore
secret.h
*.pem
*.key
.env
.env.*
```

### 6. Usare un template sicuro
Mantieni nella repo solo un file di esempio, ad esempio `secret.example.h`, con valori fittizi.

Esempio:
```cpp
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
#define API_KEY "your-api-key"
```

E tieni il file reale `secret.h` solo in locale.

### 7. Bonificare la cronologia Git se necessario
Se i segreti sono stati committati, valuta la riscrittura della history per rimuoverli.

Nota:
- anche dopo la bonifica, le credenziali vanno comunque ruotate
- la bonifica serve a ridurre l'esposizione futura, non a rendere sicuri segreti già pubblicati

### 8. Controllare anche file non ovvi
In questa repo ci sono molti notebook e file dati. Verifica anche:
- output notebook
- celle salvate con credenziali
- path locali
- email personali
- dump di debug
- screenshot o immagini contenenti dati sensibili
- CSV esportati con dati personali o identificativi

### 9. Ridurre i dati personali pubblici
Controlla la presenza di:
- email personali
- numeri di telefono
- nomi completi
- percorsi locali del PC
- metadata dentro notebook o documenti

### 10. Impostare una regola per il futuro
Prima di ogni push:
- controlla `git diff`
- controlla `git status`
- evita commit di file locali o temporanei
- usa file separati per i segreti
- mantieni solo template di configurazione nella repo

---

## Checklist operativa

- [ ] Verificare se `secret.h` è tracciato nella repo
- [ ] Controllare `SleepSense.ino`
- [ ] Controllare `.gitignore`
- [ ] Cercare password, token, API key e segreti nei file di progetto
- [ ] Cercare segreti nei notebook `.ipynb`
- [ ] Cercare dati personali in file `.txt`, `.csv`, immagini e documenti
- [ ] Ruotare tutte le credenziali eventualmente esposte
- [ ] Sostituire i file reali con template di esempio
- [ ] Escludere i file sensibili con `.gitignore`
- [ ] Valutare bonifica della cronologia Git
- [ ] Controllare branch, tag e fork

---

## File da mantenere nella repo

Consentiti:
- `secret.example.h`
- documentazione
- file di configurazione senza credenziali reali

Da non committare:
- `secret.h`
- `.env`
- chiavi private
- certificati privati
- backup locali
- dump con dati sensibili

---

## Nota finale
Il commit trovato è un segnale concreto che la repo ha probabilmente contenuto segreti. Le credenziali coinvolte devono essere considerate compromesse fino a prova contraria.
