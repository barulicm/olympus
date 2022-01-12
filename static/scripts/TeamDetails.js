function onLoad() {
    var current_url = window.location.href;
    var team_number = current_url.substr(current_url.indexOf("?team=")+6);

    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/'+team_number,true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState == 4 && xhr.status == 200) {
            var team = JSON.parse(xhr.responseText);

            document.getElementById("teamNumber").value = team.number;
            document.getElementById("teamName").value = team.name;

            var scoresArr = team.scores[0];

            document.getElementById("numberCell").colSpan = scoresArr.length;
            document.getElementById("nameCell").colSpan = scoresArr.length;
            document.getElementById("submitCell").colSpan = scoresArr.length;

            var scoresRow = document.getElementById("scoresRow");
            var i;
            for(i = 0; i < scoresArr.length; i++) {
                var cell = document.createElement("td");

                var label = document.createElement("label");
                label.className = "grouptitle";
                label.appendChild(document.createTextNode("Match " + (i+1) + " Score"));
                cell.appendChild(label);

                var input = document.createElement("input");
                input.type = "number";
                input.value = scoresArr[i];
                cell.appendChild(input);

                scoresRow.appendChild(cell);
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
    let i;
    for(i = 0; i < scoreCells.length; i++) {
        let score = parseInt(scoreCells[i].getElementsByTagName("input")[0].value);
        jsonData.newScores.push(score);
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