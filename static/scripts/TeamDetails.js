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

            let scoresArr = team.scores;
            let gpScoresArr = team.gpScores;

            document.getElementById("numberCell").colSpan = scoresArr.length;
            document.getElementById("nameCell").colSpan = scoresArr.length;
            document.getElementById("submitCell").colSpan = scoresArr.length;

            let scoresRow = document.getElementById("scoresRow");
            for(let i = 0; i < scoresArr.length; i++) {
                let cell = document.createElement("td");

                let label = document.createElement("label");
                label.className = "grouptitle";
                label.appendChild(document.createTextNode("Match " + (i+1) + " Score"));
                cell.appendChild(label);

                let input = document.createElement("input");
                input.type = "number";
                input.value = scoresArr[i];
                cell.appendChild(input);

                scoresRow.appendChild(cell);
            }

            let gpScoresRow = document.getElementById("gpScoresRow");
            for(let i = 0; i < gpScoresArr.length; i++) {
                let cell = document.createElement("td");

                let label = document.createElement("label");
                label.className = "grouptitle";
                label.appendChild(document.createTextNode("Match " + (i+1) + " GP Score"));
                cell.appendChild(label);

                let input = document.createElement("input");
                input.type = "number";
                input.value = gpScoresArr[i];
                cell.appendChild(input);

                gpScoresRow.appendChild(cell);
            }
        }
    }
}

function submitEdits() {
    let jsonData = {}

    let current_url = window.location.href;
    jsonData.oldTeamNumber = current_url.substr(current_url.indexOf("?team=")+6);
    jsonData.newTeamNumber = document.getElementById("teamNumber").value;
    jsonData.newTeamName   = document.getElementById("teamName").value;

    jsonData.newScores = [];
    let scoreCells = document.getElementById("scoresRow").childNodes;
    for(let i = 0; i < scoreCells.length; i++) {
        let score = parseInt(scoreCells[i].getElementsByTagName("input")[0].value);
        jsonData.newScores.push(score);
    }

    jsonData.newGPScores = [];
    let gpScoreCells = document.getElementById("gpScoresRow").childNodes;
    for (let i = 0; i < gpScoreCells.length; i++) {
        let score = parseInt(gpScoreCells[i].getElementsByTagName("input")[0].value);
        jsonData.newGPScores.push(score);
    }

    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'team/edit', true);
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                alert("Edits Saved");
                location.reload();
            } else {
                alert("Submitting team edits failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}