# Progetto IoT – SleepSense ESP32

## Descrizione

SleepSense è un progetto IoT per il monitoraggio e la classificazione della qualità del sonno, basato su microcontrollore ESP32 e sensori ambientali e di movimento. Il sistema raccoglie dati multimodali e li integra per fornire una valutazione sintetica e oggettiva del riposo notturno.

## Obiettivi

- Monitorare parametri ambientali e comportamentali durante il sonno  
- Integrare sensori eterogenei in un’unica pipeline dati  
- Classificare la qualità del sonno tramite regole e analisi successive  
- Fornire una base scalabile per sviluppi futuri (ML, dashboard, cloud)

## Hardware utilizzato

- ESP32  
- Sensore di umidità  
- Sensore di luminosità  
- Microfono ambientale  
- Sensore di movimento
- Schermo OLED

## Architettura del sistema

1. Acquisizione dati dai sensori  
2. Pre-elaborazione locale su ESP32  
3. Trasmissione e salvataggio dei dati  
4. Analisi e classificazione della qualità del sonno  

## Stato del progetto

Prototipo funzionante e testato in ambiente controllato.  
In fase di analisi delle limitazioni e definizione delle implementazioni future.

## Sviluppi futuri

- Integrazione di modelli di machine learning  
- Miglioramento della precisione dei sensori  
- Visualizzazione dei dati tramite dashboard  
- Ottimizzazione dei consumi energetici  

## Criticità

- Misurazione errata del rumore
- Misurazione errata del movimento

## Autore

Matteo Leopizzi

## Licenza

Progetto sviluppato a scopo accademico e sperimentale. Università di Urbino
