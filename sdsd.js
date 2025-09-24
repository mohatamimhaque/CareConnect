          fetch('http://localhost/careConnect/store.php', {
                                method: 'POST',
                                headers: {
                                    'Content-Type': 'application/json',
                                },
                                body: JSON.stringify({ dhtTemp: data.dhtTemp !== null ? data.dhtTemp.toFixed(1) : 0,
                                    humidity: data.humidity !== null ? data.humidity.toFixed(1) : 0,
                                    bodyTemp: data.bodyTemp !== null ? data.bodyTemp.toFixed(1) : 0,
                                    bpm: (data.bpm >= 50 && data.bpm <= 200 && data.spo2 >= 60 && data.spo2 <= 100) ? data.bpm.toFixed(1) : 0,
                                    spo2: (data.bpm >= 50 && data.bpm <= 200 && data.spo2 >= 60 && data.spo2 <= 100) ? data.spo2.toFixed(1) : 0
                                }),
                            })
                                .then(response => response.json())
                                .then(data => {
                                })
                                .catch(error => {
                                    console.error('Error updating count:', error);
                                });                    

