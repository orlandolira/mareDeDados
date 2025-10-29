CREATE DATABASE sensor_data;

USE sensor_data;

CREATE TABLE sensor_readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    location VARCHAR(50),
    temperature FLOAT,
    humidity FLOAT,
    co2 INT,
    air_quality INT,
    wifi_strength INT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_timestamp ON sensor_readings(timestamp);
CREATE INDEX idx_device_id ON sensor_readings(device_id);