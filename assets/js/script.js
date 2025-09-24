 let healthChart;

 async function fetchData() {
     const filter = document.getElementById("filter").value;
     const response = await fetch(`http://localhost/careConnect/fetch_data.php?filter=${filter}`);
     const data = await response.json();

     const labels = data.map(item => item.time_group);
     const dhtTemp = data.map(item => parseFloat(item.dhtTemp));
     const humidity = data.map(item => parseFloat(item.humidity));
     const bodyTemp = data.map(item => parseFloat(item.bodyTemp));
     const bpm = data.map(item => parseFloat(item.bpm));
     const spo2 = data.map(item => parseFloat(item.spo2));

     updateChart(labels, dhtTemp, humidity, bodyTemp, bpm, spo2);
 }

 function updateChart(labels, dhtTemp, humidity, bodyTemp, bpm, spo2) {
     if (healthChart) {
         healthChart.destroy();
     }

     healthChart = new Chart(document.getElementById("healthChart"), {
         type: 'line',
         data: {
             labels: labels,
             datasets: [{
                     label: "DHT Temp (°C)",
                     data: dhtTemp,
                     borderColor: "#ff6384",
                     backgroundColor: "transparent",
                     tension: 0.3
                 },
                 {
                     label: "Humidity (%)",
                     data: humidity,
                     borderColor: "#36a2eb",
                     backgroundColor: "transparent",
                     tension: 0.3
                 },
                 {
                     label: "Body Temp (°C)",
                     data: bodyTemp,
                     borderColor: "#ff9f40",
                     backgroundColor: "transparent",
                     tension: 0.3
                 },
                 {
                     label: "BPM",
                     data: bpm,
                     borderColor: "#4bc0c0",
                     backgroundColor: "transparent",
                     tension: 0.3
                 },
                 {
                     label: "SpO2 (%)",
                     data: spo2,
                     borderColor: "#9966ff",
                     backgroundColor: "transparent",
                     tension: 0.3
                 }
             ]
         },
         options: {
             responsive: true,
             plugins: {
                 legend: {
                     labels: {
                         color: '#fff'
                     }
                 },
                 tooltip: {
                     mode: 'index',
                     intersect: false
                 }
             },
             scales: {
                 x: {
                     ticks: {
                         color: '#ccc'
                     },
                     grid: {
                         color: '#333'
                     },
                     title: {
                         display: true,
                         text: 'Time',
                         color: '#aaa'
                     }
                 },
                 y: {
                     ticks: {
                         color: '#ccc'
                     },
                     grid: {
                         color: '#333'
                     },
                     title: {
                         display: true,
                         text: 'Sensor Values',
                         color: '#aaa'
                     }
                 }
             }
         }
     });
 }

 fetchData();