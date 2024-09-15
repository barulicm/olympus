function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var datalist = document.getElementById("teams");
            var teams = JSON.parse(xhr.responseText);
            teams.sort((a,b) => parseInt(a.number) - parseInt(b.number))
            teams.forEach((team) => {
                let option = document.createElement("option");
                option.setAttribute("value", team.number + ' - ' + team.name);
                datalist.appendChild(option);
            });
        }
    }
}

function submitScore(team,score,gpScore) {
    let jsonData = {};
    let endOfTeamNumber = team.indexOf(" ");
    jsonData.teamNumber = team.substring(0,endOfTeamNumber);
    jsonData.score = score;
    jsonData.gpScore = parseInt(gpScore);

    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/submit', true);
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                document.body.scrollTop = document.documentElement.scrollTop = 0;
                location.reload();
            } else {
                alert("Submitting score failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}

function onTeamChange(teamText) {
    let endOfTeamNumber = teamText.indexOf(" ");
    let teamNumber = teamText.substring(0,endOfTeamNumber);
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'team/'+teamNumber, true);
    xhr.send();
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            let team = JSON.parse(xhr.responseText);
            var output = document.getElementById("matchOutput");
            output.value = team.scores.length+1;
        }
    }
}