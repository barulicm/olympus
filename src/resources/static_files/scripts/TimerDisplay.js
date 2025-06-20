function onWebsocketMessage(event) {
    let timer_json = JSON.parse(event.data);
    let time_remaining = timer_json.time_remaining;
    let timer_minutes = Math.floor(time_remaining / 60);
    let timer_seconds = time_remaining % 60;
    let element = document.getElementById('timerDisplay');
    element.innerText = timer_minutes.toString() + ":" + timer_seconds.toString().padStart(2, '0');
    element.style.color = 'black';
}

function onWebsocketClose(event) {
    console.log("WebSocket closed:", event.reason);
    let element = document.getElementById('timerDisplay');
    element.innerText = "-:--";
    element.style.color = 'red';
    setTimeout(() => {
        console.log("Attempting to reconnect...");
        openWebsocket();
    }, 1_000);
}

function onWebsocketError(error) {
    console.error("WebSocket error:", error);
}

function openWebsocket() {
    const socket = new WebSocket("ws://" + window.location.host + "/timer/ws");
    socket.onmessage = onWebsocketMessage;
    socket.onclose = onWebsocketClose;
    socket.onerror = onWebsocketError;
}

function onLoad() {
    openWebsocket();
}
