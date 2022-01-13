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

function queryHasSchedule() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','controlQuery',true);
    xhr.setRequestHeader('query','hasSchedule');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            if (xhr.responseText === 'true') {
                document.getElementById('scheduleControls').className += ' disabledcontrol';
            }
        }
    }
}

function queryHasNextMatch() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','controlQuery',true);
    xhr.setRequestHeader('query','hasNextMatch');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            if (xhr.responseText === 'true') {
                document.getElementById('nextButton').innerText = 'Next Match';
            }
        }
    }
}

function queryHasNextPhase() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','controlQuery',true);
    xhr.setRequestHeader('query','hasNextPhase');
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            if (xhr.responseText === 'true') {
                document.getElementById('nextButton').innerText = 'Next Phase';
            } else {
                document.getElementById('nextButton').className += ' disabledcontrol';
            }
        }
    }
}

function getDynamicPageList() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','pages/dynamic',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var pageArr = JSON.parse(xhr.responseText);
            var pageInd;
            var pageListDiv = document.getElementById("dynamicPageList");
            for(pageInd = 0; pageInd < pageArr.length; pageInd++) {
                var linkElement = document.createElement("a");
                linkElement.setAttribute("href",pageArr[pageInd] + ".html");
                linkElement.innerText = pageArr[pageInd];
                pageListDiv.appendChild(linkElement);
            }
        }
    }
}

function getCurrentMatchInfo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','schedule/match/current',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var matchInfo = JSON.parse(xhr.responseText);
            var currentMatchDisplay = document.getElementById("currentMatchDisplay");
            var phaseName = "Phase";
            if("phase" in matchInfo) {
                phaseName = matchInfo.phase;
            }
            var matchNumber = "_";
            if("number" in matchInfo) {
                matchNumber = (parseInt(matchInfo.number)+1).toString();
            }
            currentMatchDisplay.innerText = phaseName + " - Match " + matchNumber;
        }
    }
}

function onLoad() {
    queryHasTeams();
    queryHasSchedule();
    queryHasNextPhase();
    queryHasNextMatch();
    getDynamicPageList();
    getCurrentMatchInfo();
    setInterval(updateTimer, 100);
}

function importTeams() {
    var files = document.getElementById('importTeamsFile').files;
    if(files.length === 0) {
        alert("Please select a file first.");
        return;
    }
    var file = files[0];
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
                var team_number = tokens[0];
                var team_name = tokens[1];
                sendAddTeam(team_name, team_number,false,false);
            }
            location.reload();
        };
    })(file);

    reader.readAsText(file);
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
                alert("Adding team " + teamNumber + " failed. Status code " + xhr.status);
            }
        }
    }
    xhr.send(jsonString);
}

function importSchedule() {
    var files = document.getElementById('importScheduleFile').files;
    if(files.length === 0) {
        alert("Please select a file first.");
        return;
    }
    var file = files[0];
    var reader = new FileReader();
    reader.onload = (function(theFile) {
        return function(e) {
            var xhr = new XMLHttpRequest();
            xhr.open('PUT','schedule/load',true);
            xhr.onreadystatechange = ()=>{
                if(xhr.readyState === 4) {
                    if(xhr.status === 200) {
                        location.reload();
                    } else {
                        alert("Loading schedule failed.\nStatus code: " + xhr.status + "\nResponse: " + xhr.responseText);
                    }
                }
            }
            xhr.send(e.target.result);
        };
    })(file);

    reader.readAsText(file);
}

function nextButtonClicked() {
    var xhr = new XMLHttpRequest();
    xhr.open('PUT','schedule/next',true);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            } else {
                alert(xhr.responseText);
            }
        }
    }
    xhr.send();
}

function exportScoresButtonClicked() {
    window.open('scores/export.csv', '_blank');
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
        timer_button.innerText = 'Start Timer';
    }
}

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
            if(time_remaining === 0 && timer_button.innerText === 'Stop Timer') {
                timer_button.innerText = 'Reset Timer';
            }
        }
    }
    xhr.send();
}
