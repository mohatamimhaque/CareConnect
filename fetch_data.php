<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

$host = "localhost";
$dbname = "careconncet";
$username = "root";
$password = "";

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    die(json_encode(["error" => "Connection failed"]));
}

$filter = $_GET['filter'] ?? 'hourly';

$filter = $_GET['filter'] ?? 'hourly';

switch ($filter) {
    case 'minutely':
        $group = "%Y-%m-%d %H:%i";
        break;
    case 'daily':
        $group = "%Y-%m-%d";
        break;
    case 'weekly':
        $group = "%Y-%u";
        break;
    case 'monthly':
        $group = "%Y-%m";
        break;
    case 'hourly':
    default:
        $group = "%Y-%m-%d %H:00:00";
        break;
}


$sql = "
    SELECT 
        DATE_FORMAT(timestamp, '$group') AS time_group,
        ROUND(AVG(dhtTemp), 2) AS dhtTemp,
        ROUND(AVG(humidity), 2) AS humidity,
        ROUND(AVG(bodyTemp), 2) AS bodyTemp,
        ROUND(AVG(bpm), 2) AS bpm,
        ROUND(AVG(spo2), 2) AS spo2
    FROM sensor_data
    GROUP BY time_group
    ORDER BY time_group DESC
    LIMIT 16
";

$result = $conn->query($sql);
$data = [];

while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

echo json_encode(array_reverse($data));
?>
