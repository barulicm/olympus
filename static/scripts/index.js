function queryHasTeams() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','controlQuery',true);
    xhr.setRequestHeader('query','hasTeams');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            if (xhr.responseText === 'false') {
                document.getElementById('scheduleControls').className += ' disabledcontrol';
            } else {
                document.getElementById('teamControls').className += ' disabledcontrol';
            }
        }
    }
}

function getShowTimer() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'show_timer');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if (xhr.status === 200) {
                document.getElementById('showTimerCheckbox').checked = xhr.responseText === 'true';
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function getRowsPerDisplay() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'rows_on_display');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                document.getElementById('rowsPerDisplay').value = parseInt(xhr.responseText);
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function onLoad() {
    queryHasTeams();
    getShowTimer();
    getRowsPerDisplay();
    setInterval(updateTimer, 100);
}

function onTeamsFileSelected(e) {
    if(e.target.files.length === 0) {
        location.reload();
    }
    var file = e.target.files[0];
    var reader = new FileReader();
    reader.onload = (function(theFile) {
        return function(e) {
            var contents = e.target.result;
            var lines = contents.split("\n");
            for(var i = 0; i < lines.length; i++) {
                if (lines[i].length === 0) {
                    // Skip empty lines
                    continue;
                }
                var tokens = lines[i].split(",");
                var team_number = tokens[0].replace(/[^0-9]/gi, '');;
                var team_name = tokens[1].replace(/[^a-z0-9 -]/gi, '');
                sendAddTeam(team_name, team_number,false,false);
            }
            location.reload();
        };
    })(file);

    reader.readAsText(file);
}

function importTeams() {
    let fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.accept = '.csv';
    fileInput.onchange = onTeamsFileSelected;
    fileInput.click();
}

function sendAddTeam(teamName, teamNumber, reload, async) {
    var jsonTeam = {};
    jsonTeam.name = teamName;
    jsonTeam.number = teamNumber;
    var jsonString = JSON.stringify(jsonTeam);

    var xhr = new XMLHttpRequest();
    xhr.open('PUT','team/add',async);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                if (reload) {
                    location.reload();
                }
            } else {
                alert("Adding team " + teamNumber + " failed. Status code " + xhr.status + '. ' + xhr.responseText);
            }
        }
    }
    xhr.send(jsonString);
}

function rerankTeamsButtonClicked() {
    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/rerank', true);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            }
        } else {
            alert("Reranking teams failed. Status code " + xhr.status + '. ' + xhr.responseText);
        }
    }
}

function exportScoresButtonClicked() {
    window.open('scores/export.csv', '_blank');
}

function exportGPScoresButtonClicked() {
    window.open('scores/export_gp.csv', '_blank');
}

function exportSessionButtonClicked() {
    window.open('session_backup.json', '_blank');
}

function importSessionButtonClicked() {
    var fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.accept = '.json';
    fileInput.onchange = e => {
        if(e.target.files.length === 0) {
            location.reload();
        }
        var reader = new FileReader();
        reader.onload = function(){
            let xhr = new XMLHttpRequest();
            xhr.open('PUT', 'session/import', true);
            xhr.onreadystatechange = ()=>{
                if(xhr.readyState === 4) {
                    if(xhr.status === 200) {
                        location.reload();
                    } else {
                        alert("Importing session. Status code " + xhr.status + '. ' + xhr.responseText);
                    }
                }
            };
            xhr.send(reader.result);
        };
        reader.readAsText(e.target.files[0]);
    };
    fileInput.click();
}

function timerButtonClicked() {
    let timer_button = document.getElementById('timerButton');
    if(timer_button.innerText === 'Start Timer') {
        let xhr = new XMLHttpRequest()
        xhr.open('PUT', 'timer/start', true);
        xhr.onreadystatechange = ()=>{
            if(xhr.readyState === 4) {
                if(xhr.status !== 200) {
                    alert(xhr.responseText);
                }
            }
        }
        xhr.send();
        timer_button.innerText = 'Stop Timer';
        document.getElementById('startAudio').play();
    } else {
        let xhr = new XMLHttpRequest()
        xhr.open('PUT', 'timer/stop', true);
        xhr.onreadystatechange = ()=>{
            if(xhr.readyState === 4) {
                if(xhr.status !== 200) {
                    alert(xhr.responseText);
                }
            }
        }
        xhr.send();
        if(timer_button.innerText === 'Stop Timer') {
            document.getElementById('stopAudio').play();
        }
        timer_button.innerText = 'Start Timer';
    }
}

let prev_time_remaining = 0;
function updateTimer() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET','timer',true);
    xhr.onreadystatechange = ()=> {
        if(xhr.readyState === 4 && xhr.status === 200) {
            let timer_json = JSON.parse(xhr.responseText);
            let time_remaining = timer_json.time_remaining;
            let timer_minutes = Math.floor(time_remaining / 60);
            let timer_seconds = time_remaining % 60;
            document.getElementById('timerDisplay').innerText = timer_minutes.toString() + ":" + timer_seconds.toString().padStart(2,'0');
            let timer_button = document.getElementById('timerButton');
            if(time_remaining === 30 && prev_time_remaining > 30) {
                document.getElementById('endGameAudio').play();
            } else if(time_remaining === 0 && prev_time_remaining > 0) {
                document.getElementById('endAudio').play();
            }
            if(time_remaining === 0 && timer_button.innerText === 'Stop Timer') {
                timer_button.innerText = 'Reset Timer';
            }
            prev_time_remaining = time_remaining;
        }
    }
    xhr.send();
}

function adjustVolumes() {
    let volume = document.getElementById('volumeRange').value / 100.0;
    document.getElementById('startAudio').volume = volume;
    document.getElementById('stopAudio').volume = volume;
    document.getElementById('endGameAudio').volume = volume;
    document.getElementById('endAudio').volume = volume;
}

function setShowTimer() {
    let show_timer = document.getElementById('showTimerCheckbox').checked;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'config', true);
    xhr.setRequestHeader('name', 'show_timer');
    xhr.setRequestHeader('value', show_timer);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
            document.getElementById('showTimerCheckbox').checked = !document.getElementById('showTimerCheckbox').checked;
        }
    }
}

function setRowsPerDisplay() {
    let value = document.getElementById('rowsPerDisplay').value;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'config', true);
    xhr.setRequestHeader('name', 'rows_on_display');
    xhr.setRequestHeader('value', value);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
        }
    }
}
