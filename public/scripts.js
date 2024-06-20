
let tasks = [];

document.addEventListener('DOMContentLoaded', (event) => {
    document.getElementById('addTaskButton').addEventListener('click', addTask);
    document.getElementById('submitTasksButton').addEventListener('click', submitTasks);
});

function addTask() {
    const taskId = document.getElementById('taskId').value;
    const arrivalTime = document.getElementById('arrivalTime').value;
    const burstTime = document.getElementById('burstTime').value;

    const task = {
        task_id: parseInt(taskId),
        arrival_time: parseInt(arrivalTime),
        burst_time: parseInt(burstTime)
    };

    tasks.push(task);
    displayTasks();
}

function displayTasks() {
    const taskList = document.getElementById('taskList');
    taskList.innerHTML = '';

    tasks.forEach((task, index) => {
        const li = document.createElement('li');
        li.innerHTML = `ID ${task.task_id} (Arrival: ${task.arrival_time}ms, Burst: ${task.burst_time}ms)`;
        const removeButton = document.createElement('button');
        removeButton.textContent = 'Remove';
        removeButton.addEventListener('click', () => removeTask(index));
        li.appendChild(removeButton);
        taskList.appendChild(li);
    });
}

function removeTask(index) {
    tasks.splice(index, 1);
    displayTasks();
}

function submitTasks() {
    fetch('http://localhost:5000/schedule', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(tasks)
    })
    .then(response => response.json())
    .then(data => {
        const output = document.getElementById('output');
        output.innerHTML = '';

        const sche1 = data.schedule[0].sche;
        const s1 = document.createElement('p');
        s1.innerHTML = `<b>Ratio based algorithm Schedule:</b><br>${sche1}`;
        output.appendChild(s1);
        const stats1 = data.schedule[1];
        const p1 = document.createElement('p');
        p1.innerHTML = `<b>Ratio based Scheduler Stats:</b> <br> <em>Average Turnaround Time</em>: ${stats1.att} <br> <em>Average Waiting Time</em>: ${stats1.awt}`;
        output.appendChild(p1);

        const sche2 = data.schedule[2].sche;
        const f = document.createElement('p');
        f.innerHTML = `<b>FCFS Schedule:</b><br>${sche2}`;
        output.appendChild(f);
        const fstats = data.schedule[3];
        const fp = document.createElement('p');
        fp.innerHTML = `<b>FCFS Stats</b> <br> <em>Average Turnaround Time</em>: ${fstats.att} <br> <em>Average Waiting Time</em>: ${fstats.awt}`;
        output.appendChild(fp);

        const sche3 = data.schedule[4].sche;
        const s3 = document.createElement('p');
        s3.innerHTML = `<b>RR Schedule:</b><br>${sche3}`;
        output.appendChild(s3);
        const rstats = data.schedule[5];
        const r = document.createElement('p');
        r.innerHTML = `<b>RR Scheduler Stats:</b> <br> <em>Average Turnaround Time</em>: ${rstats.att} <br> <em>Average Waiting Time</em>: ${rstats.awt}`;
        output.appendChild(r);

        const sche4 = data.schedule[6].sche;
        const s4 = document.createElement('p');
        s4.innerHTML = `<b>SJF Schedule:</b><br>${sche4}`;
        output.appendChild(s4);
        const jstats = data.schedule[7];
        const jp = document.createElement('p');
        jp.innerHTML = `<b>SJF Stats</b> <br> <em>Average Turnaround Time</em>: ${jstats.att} <br> <em>Average Waiting Time</em>: ${jstats.awt}`;
        output.appendChild(jp);
    });
}
