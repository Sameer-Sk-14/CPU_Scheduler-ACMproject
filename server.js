const express = require('express');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 5000;

app.use(express.static('public'));

app.use(bodyParser.json());

app.post('/schedule', (req, res) => {
    const tasks = req.body;

    let inputData = tasks.map(task => `${task.task_id} ${task.arrival_time} ${task.burst_time}`).join('\n');
    
    const inputFilePath = path.join(__dirname, 'input.txt');
    fs.writeFileSync(inputFilePath, inputData);

    const schedulerPath = path.join(__dirname, 'scheduler.exe');

    const scheduler = spawn(schedulerPath, [], { stdio: ['pipe', 'pipe', 'pipe'] });

    const inputStream = fs.createReadStream(inputFilePath);
    inputStream.pipe(scheduler.stdin);

    let output = '';
    scheduler.stdout.on('data', (data) => {
        output += data.toString();
    });

    scheduler.stderr.on('data', (data) => {
        console.error(`Scheduler stderr: ${data}`);
    });

    scheduler.on('close', (code) => {
        if (code !== 0) {
            return res.status(500).send('Error executing scheduler');
        }
        let outputLines = output.trim().split('\n');
        let schedule = outputLines.map((line,index) => {
            if (index === 0||index===2||index===4||index===6) {
                return {
                    sche: line
                };
            } else {
            let parts = line.split(' ');
            return {
                att: parseFloat(parts[3]),
                awt: parseFloat(parts[7])
            };
            }
        });

        res.json({ schedule });
    });
});

app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});
