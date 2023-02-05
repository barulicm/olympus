const DisplayStates = {
    ShowScores: "ShowScores",
    Blackout: "Blackout",
    FllLogo: "FllLogo"
}

let current_top_team = 0;
let teams_per_page = 8;
let display_state = DisplayStates.ShowScores;

function getInfo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var teamArr = JSON.parse(xhr.responseText);

            teamArr.sort(function(a, b){
                var keyA = a.rank;
                var keyB = b.rank;
                if(keyA < keyB) return -1;
                if(keyA > keyB) return 1;
                return 0;
            });

            let round_count = Math.max(... teamArr.map(team => team.scores.length))

            let tabBody = document.getElementsByTagName("tbody").item(0);
            while(tabBody.firstChild) {
                tabBody.removeChild(tabBody.lastChild);
            }

            let headerRow = document.createElement("tr");

            let rankHeader = document.createElement("th");
            rankHeader.innerText = "Rank";
            rankHeader.attributes["scope"] = "col";
            rankHeader.classList.add("threeStud");
            rankHeader.id = "rankHeader";
            headerRow.appendChild(rankHeader);

            let teamNameHeader = document.createElement("th");
            teamNameHeader.innerText = "Team";
            teamNameHeader.attributes["scope"] = "col";
            teamNameHeader.id = "teamNameHeader";
            headerRow.appendChild(teamNameHeader);

            for(let i = 0; i < round_count; i++) {
                let roundHeader = document.createElement("th");
                roundHeader.innerText = "R" + (i+1);
                roundHeader.attributes["scope"] = "col";
                roundHeader.classList.add("RoundHeader");
                headerRow.appendChild(roundHeader);
            }

            let finalScoreHeader = document.createElement("th");
            finalScoreHeader.innerText = "Final";
            finalScoreHeader.attributes["scope"] = "col";
            finalScoreHeader.classList.add("threeStud");
            finalScoreHeader.id = "finalScoreHeader";
            headerRow.appendChild(finalScoreHeader);

            tabBody.appendChild(headerRow);

            for(let i = current_top_team; i  < Math.min(current_top_team+teams_per_page,teamArr.length); i++) {
                // Populate Table Row
                row=document.createElement("tr");

                rankCell=document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                row.appendChild(rankCell);

                teamCell=document.createElement("td");
                teamCell.style.whiteSpace='pre';
                // teamCell.appendChild(document.createTextNode(teamArr[i].number.padStart(5, ' ') + "  " + teamArr[i].name.substr(0,10)));
                teamCell.appendChild(document.createTextNode(teamArr[i].number.padStart(5, ' ') + "  " + teamArr[i].name));
                row.appendChild(teamCell);

                for(let r = 0; r < round_count; r++) {
                    roundCell = document.createElement("td");
                    if(teamArr[i].scores.length > r) {
                        roundCell.appendChild(document.createTextNode(teamArr[i].scores[r]));
                    } else {
                        roundCell.appendChild(document.createTextNode(" "));
                    }
                    row.appendChild(roundCell);
                }

                scoreCell=document.createElement("td");
                scoreCell.appendChild(document.createTextNode(teamArr[i].displayScore));
                row.appendChild(scoreCell);

                tabBody.appendChild(row);
            }

            current_top_team += teams_per_page;
            if(current_top_team >= teamArr.length) {
                current_top_team = 0;
            }
        }
    };
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
        }
    }
    xhr.send();
}

function setBlackoutTop(val_in_studs) {
    let blackoutElement = document.getElementById("blackout");
    blackoutElement.style.setProperty("top", "calc(" + val_in_studs + " * var(--stud-size))");
}

function setFllLogoVisibility(visible) {
    let fllLogoElement = document.getElementById("fllLogoContainer");
    if(visible === true) {
        fllLogoElement.style.setProperty("top", "0");
    } else {
        fllLogoElement.style.setProperty("top", "100%");
    }
}

function getDisplayState() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'display_state');
    xhr.send();
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                display_state = xhr.responseText;
                switch (display_state) {
                    case DisplayStates.ShowScores:
                        setBlackoutTop(teams_per_page + 4);
                        setFllLogoVisibility(false);
                        break;
                    case DisplayStates.Blackout:
                        setBlackoutTop(0);
                        setFllLogoVisibility(false);
                        break;
                    case DisplayStates.FllLogo:
                        setFllLogoVisibility(true);
                        break;
                }
            } else {
                console.error('Could not get display state config: ' + xhr.responseText);
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
                if(xhr.responseText === 'false') {
                    document.getElementById('timerDisplay').style.visibility = 'hidden';
                } else {
                    document.getElementById('timerDisplay').style.visibility = 'visible';
                }
            } else {
                console.error('Could not get timer config: ' + xhr.responseText);
            }
        }
    }
}

function getTeamsPerPage() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'rows_on_display');
    xhr.send();
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                teams_per_page = parseInt(xhr.responseText);
                if(display_state === DisplayStates.ShowScores) {
                    setBlackoutTop(teams_per_page + 4);
                }
            } else {
                console.error('Could not get rows config: ' + xhr.responseText);
            }
        }
    }
}

function updateDisplayConfig() {
    getDisplayState();
    getShowTimer();
    getTeamsPerPage();
}

function onLoad() {
    updateDisplayConfig();
    setInterval(updateDisplayConfig, 1000);
    getInfo();
    setInterval(getInfo, 5000);
    updateTimer();
    setInterval(updateTimer, 100);
}