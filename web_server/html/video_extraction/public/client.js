let eventSource;
const clientId = Math.floor(Math.random() * 100000);

document.getElementById('startBtn').onclick = () => {
    const camera = document.getElementById('camera').value;
    const module = document.getElementById('module').value;
    const startTime = document.getElementById('startTime').value;
    const startDate = document.getElementById('startDay').value;
    const stopTime = document.getElementById('stopTime').value;
    const stopDate = document.getElementById('stopDay').value;

    if (!startTime || !stopTime) { alert('Choisissez les deux horaires'); return; }

    const output = document.getElementById('output');
    output.textContent = '';
    document.getElementById('startBtn').disabled = true;
    document.getElementById('stopBtn').disabled = false;

    eventSource = new EventSource(`/download_video/run?module=${module}&camera=${camera}&start_date=${startDate}&start_time=${startTime}&stop_date=${stopDate}&stop_time=${stopTime}`);
    eventSource.onmessage = (e) => {
        output.innerHTML += e.data + '</br>';
        output.scrollTop = output.scrollHeight;
    };
    eventSource.onerror = () => {
        eventSource.close();
        document.getElementById('startBtn').disabled = false;
        document.getElementById('stopBtn').disabled = true;
    };
};

document.getElementById('stopBtn').onclick = () => {
    fetch(`/stop?clientId=${clientId}`)
        .then(() => {
            if (eventSource) eventSource.close();
            document.getElementById('startBtn').disabled = false;
            document.getElementById('stopBtn').disabled = true;
        });
};
