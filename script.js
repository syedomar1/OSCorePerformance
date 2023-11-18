const express = require('express');
const { exec } = require('child_process');
const app = express();
const port = 3000;

app.use(express.urlencoded({ extended: true }));
app.use(express.json());

app.use(express.static('public'));

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});

app.get('/run-cpp/:no_cores', (req, res) => {
    const noCores = req.params.no_cores;
    exec(`./runner ${noCores}`, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${error}`);
            return res.status(500).send('Internal Server Error');
        }
        const output = stdout;
        res.send(output);
    });
});

app.post('/execute', (req, res) => {
    const noCores = req.body.noCores;

    exec(`./scheduler.exe ${noCores}`, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${error}`);
            return res.status(500).json({ error: 'Internal Server Error' });
        }
        res.json({ result: stdout });
    });
});

app.listen(port, () => {
    console.log(`Server is listening at http://localhost:${port}`);
});

const { exec } = require('child_process');

exec('./your_cpp_executable', (error, stdout, stderr) => {
    if (error) {
        console.error(`Error: ${error.message}`);
        return;
    }

    const executionTimes = stdout.split('\n')
        .filter(line => line.startsWith('Execution time'))
        .map(line => line.split(': ')[1].trim());

    console.log('Execution Times:', executionTimes);
});
function runCpp() {
    var noCores = document.getElementById('noCores').value;

    fetch('/run-cpp/' + noCores)
        .then(response => response.text())
        .then(data => {
            document.getElementById('output').innerText = data;
        })
        .catch(error => console.error('Error:', error));
}