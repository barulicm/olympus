function onLoad() {
    let current_url = window.location.href;
    let team_number = current_url.substr(current_url.indexOf("?team=")+6);

    let xhr = new XMLHttpRequest();
    xhr.open('GET','team/'+team_number,true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            let team = JSON.parse(xhr.responseText);

            document.getElementById("teamNumber").value = team.number;
            document.getElementById("teamName").value = team.name;
            document.getElementById("tournament").value = team.tournament;

            let scoresArr = team.scores;
            let gpScoresArr = team.gpScores;

            document.getElementById("numberCell").colSpan = scoresArr.length;
            document.getElementById("nameCell").colSpan = scoresArr.length;
            document.getElementById("submitCell").colSpan = scoresArr.length;

            match_count = Math.max(scoresArr.length, gpScoresArr.length);

            match_row_template = document.getElementById("matchRowTemplate");

            for(let i = 0; i < match_count; i++) {
                match_row = document.importNode(match_row_template.content, true);

                match_row.getElementById("matchRow").id += `${i}`
                let score_cell = match_row.getElementById("scoreCell");
                score_cell.id += `${i}`
                score_cell.querySelector("input").value = scoresArr[i];
                let gp_cell = match_row.getElementById("gpCell");
                gp_cell.id += `${i}`
                gp_cell.querySelector("input").value = gpScoresArr[i];

                let del_button = match_row.querySelector("button");
                del_button.onclick = (e) => removeMatch(i);

                document.getElementById("matchTableBody").appendChild(match_row);
            }
        }
    }
}

function removeMatch(index) {
    let row = document.querySelector(`#matchRow${index}`)
    let label = document.createElement("t");
    label.innerText = "Match removed";
    label.style = "color: darkRed;";
    let cell = document.createElement("td");
    cell.appendChild(label);
    row.innerHTML = "";
    row.appendChild(cell);
}

function submitEdits() {
    let jsonData = {}

    let current_url = window.location.href;
    jsonData.oldTeamNumber = current_url.substr(current_url.indexOf("?team=")+6);
    let newTeamNumber = document.getElementById("teamNumber").value;
    jsonData.newTeamNumber = newTeamNumber;
    jsonData.newTeamName   = document.getElementById("teamName").value;
    jsonData.newTournament = document.getElementById("tournament").value;

    jsonData.newScores = [];
    let scoreCells = document.querySelectorAll('[id^="scoreCell"]');
    for(let i = 0; i < scoreCells.length; i++) {
        let scoreInput = scoreCells[i].querySelector("input");
        if(scoreInput != null) {
            let score = parseInt(scoreInput.value);
            jsonData.newScores.push(score);
        }
    }

    jsonData.newGPScores = [];
    let gpScoreCells = document.querySelectorAll('[id^="gpCell"]');
    for (let i = 0; i < gpScoreCells.length; i++) {
        let scoreInput = gpScoreCells[i].querySelector("input");
        if(scoreInput != null) {
            let score = parseInt(scoreInput.value);
            jsonData.newGPScores.push(score);
        }
    }

    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'team/edit', true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                alert("Edits Saved");
                location.replace('team_details.html?team=' + newTeamNumber);
            } else {
                alert("Submitting team edits failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}