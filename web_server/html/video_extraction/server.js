const express = require('express');
const { spawn } = require('child_process');
const app = express();
const port = 3000;

app.use(express.static(__dirname + '/public'));

app.get('/', (req, res) => {
	res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// On stocke les processus par ID de client (ici, on utilise un compteur simple)

let nextClientId = 1;
const processes = {};

app.get('/run', (req, res) => {
    const { camera, module, start_date, start_time, stop_date, stop_time } = req.query;
    
    if (!camera) return res.status(400).send('Paramètre camera manquant');
    if (!module) return res.status(400).send('Paramètre module manquant');
    if (!start_date) return res.status(400).send('Paramètre start_date manquant');
    if (!start_time) return res.status(400).send('Paramètre start_time manquant');
    if (!stop_date) return res.status(400).send('Paramètre stop_date manquant');
    if (!stop_time) return res.status(400).send('Paramètre stop_time manquant');

    const clientId = nextClientId++;
    res.setHeader('Content-Type', 'text/event-stream');
    res.setHeader('Cache-Control', 'no-cache');
    res.setHeader('Connection', 'keep-alive');

    // Lancer le programme pour ce client
    const prog = spawn('/usr/local/bin/sblv_export', [module, camera, start_date, start_time, stop_date, stop_time]);
    processes[clientId] = prog;

    prog.stdout.on('data', data => {
	let lines = data.toString().split('\n');
        lines.forEach(line => res.write(`data: ${line}\n\n`));
    });

    prog.stderr.on('data', data => {
	let lines = data.toString().split('\n');
        lines.forEach(line => res.write(`data: [ERROR] ${line}\n\n`));
    });

    prog.on('close', code => {
        res.write(`data: Programme terminé avec le code ${code}\n\n`);
        res.end();
        delete processes[clientId];
    });

    // Permet de stopper le programme via /stop?clientId=...
    req.on('close', () => {
        if (processes[clientId]) {
            processes[clientId].kill();
            delete processes[clientId];
        }
    });
});

app.get('/stop', (req, res) => {
    const clientId = parseInt(req.query.clientId);
    if (clientId && processes[clientId]) {
        processes[clientId].kill();
        delete processes[clientId];
        res.send('Programme arrêté');
    } else {
        res.send('Aucun programme en cours pour ce client');
    }
});

app.listen(port, () => {
    console.log(`Serveur multi-utilisateurs lancé sur http://localhost:${port}`);
});
