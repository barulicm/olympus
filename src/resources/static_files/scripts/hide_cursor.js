let cursorHideTimeoutId;
const cursorHideDelay = 1000;  // ms

function hideCursor() {
    if(document.body !== null) {
        document.body.style.cursor = 'none';
    }
}

function resetInactivityTimer() {
    clearTimeout(cursorHideTimeoutId);
    if(document.body !== null) {
        document.body.style.cursor = 'default';
    }
    cursorHideTimeoutId = setTimeout(hideCursor, cursorHideDelay);
}

window.addEventListener('mousemove', resetInactivityTimer);
window.addEventListener('keypress', resetInactivityTimer);
window.addEventListener('click', resetInactivityTimer);

resetInactivityTimer();
