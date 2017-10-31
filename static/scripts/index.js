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

function onLoad() {
    queryHasTeams();
    queryHasSchedule();
    queryHasNextPhase();
    queryHasNextMatch();
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
