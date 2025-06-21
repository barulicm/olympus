timer_prev_time_remaining = -1;

function openTimerWebsocket(display_element_name, warningAudioName = null, endAudioName = null, timerButtonName = null) {
  const socket = new WebSocket("ws://" + window.location.host + "/timer/ws");
  socket.onmessage = (event) => {
    let timer_json = JSON.parse(event.data);
    let time_remaining = timer_json.time_remaining;
    let timer_minutes = Math.floor(time_remaining / 60);
    let timer_seconds = time_remaining % 60;
    let element = document.getElementById(display_element_name);
    element.innerText = timer_minutes.toString() + ":" + timer_seconds.toString().padStart(2, '0');
    element.style.color = 'black';
    if (timerButtonName != null && time_remaining === 0 && timer_prev_time_remaining != 0) {
      document.getElementById(timerButtonName).innerText = 'Reset Timer';
    }
    if (warningAudioName != null && time_remaining === 30 && timer_prev_time_remaining > 30) {
      document.getElementById(warningAudioName).play();
    }
    if (endAudioName != null && time_remaining === 0 && timer_prev_time_remaining > 0) {
      document.getElementById(endAudioName).play();
    }
    timer_prev_time_remaining = time_remaining;
  };
  socket.onclose = (event) => {
    console.log("Timer WebSocket closed:", event.reason);
    let element = document.getElementById(display_element_name);
    element.innerText = '-:--';
    element.style.color = 'red';
    setTimeout(() => {
      console.log("Attempting to reconnect timer websocket...");
      openTimerWebsocket(display_element_name, warningAudioName, endAudioName, timerButtonName);
    }, 1_000);
  };
  socket.onerror = (error) => {
    console.error("Timer WebSocket error:", error);
  };
}
