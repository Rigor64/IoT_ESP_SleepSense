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
