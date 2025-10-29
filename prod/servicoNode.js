// server.js - Exemplo de servidor Node.js
const express = require('express');
const mysql = require('mysql2');
const app = express();
const port = 3000;

app.use(express.json());

// Configuração do banco de dados MySQL
const db = mysql.createConnection({
  host: 'localhost',
  user: 'seu_usuario',
  password: 'sua_senha',
  database: 'sensor_data'
});

db.connect((err) => {
  if (err) throw err;
  console.log('Conectado ao banco de dados MySQL');
});

// Rota para receber dados dos sensores
app.post('/api/sensordata', (req, res) => {
  const data = req.body;
  
  console.log('Dados recebidos:', data);
  
  // Inserir no banco de dados
  const sql = `
    INSERT INTO sensor_readings 
    (device_id, location, temperature, humidity, co2, air_quality, wifi_strength, timestamp) 
    VALUES (?, ?, ?, ?, ?, ?, ?, ?)
  `;
  
  const values = [
    data.device_id,
    data.location,
    data.sensors.dht22.temperature,
    data.sensors.dht22.humidity,
    data.sensors.mhz14a.co2_ppm,
    data.sensors.mp503.quality_percent,
    data.wifi_strength,
    new Date()
  ];
  
  db.execute(sql, values, (err, result) => {
    if (err) {
      console.error('Erro ao inserir dados:', err);
      res.status(500).json({error: 'Erro no servidor'});
    } else {
      console.log('Dados inseridos com sucesso');
      res.status(201).json({message: 'Dados recebidos e salvos', id: result.insertId});
    }
  });
});

app.listen(port, () => {
  console.log(`Servidor rodando na porta ${port}`);
});