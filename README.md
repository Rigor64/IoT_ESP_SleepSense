http://informatica-iot.freeddns.org:8086/

user:iot 		
password: esp


QUERY standard influx DB

from(bucket: "esercitazioni")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r["_measurement"] == "corso_IoT")
  |> filter(fn: (r) => r["_field"] == "humidity" or r["_field"] == "is_moving" or r["_field"] == "light" or r["_field"] == "mic" or r["_field"] == "temperature")
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: true)
  |> yield(name: "mean")

Query CSV style 

from(bucket: "esercitazioni")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r["_measurement"] == "corso_IoT")
  |> filter(fn: (r) => r["_field"] == "humidity" or r["_field"] == "is_moving" or r["_field"] == "light" or r["_field"] == "mic" or r["_field"] == "temperature")
  |> aggregateWindow(every: v.windowPeriod, fn: mean, createEmpty: true)
  |> pivot(rowKey:["_time"], columnKey:["_field"], valueColumn:"_value")
  |> filter(fn: (r) => exists r.humidity and exists r.is_moving and exists r.light and exists r.mic and exists r.temperature)
  |> yield(name: "mean")



Micro-risvegli e agitazione:
variazioni frequenti in a_mag o g_mag indicano un sonno frammentato → qualità ridotta.

Cambi di postura:
l’accelerometro sugli assi mostra rotazioni lente e sostenute → segnale tipico di passaggio da REM a fasi più leggere.

Tremori o movimenti irregolari:
il giroscopio cattura vibrazioni rapide (es. scatti delle gambe, movimenti notturni inconsci) → potenziale disturbo.

Stabilità posturale:
se ax, ay, az restano stabili a lungo e gx, gy, gz ≈ 0 → sonno profondo.
Al contrario, valori irregolari e continui → agitazione.

Correlazione con dati ambientali:

Rumore ↑ + g_mag ↑ → probabile risveglio causato da disturbo acustico.

Luce ↑ + a_mag ↑ → movimenti dovuti a esposizione luminosa.

Umidità/temperatura anomale + restless sleep → ambiente sfavorevole al sonno.