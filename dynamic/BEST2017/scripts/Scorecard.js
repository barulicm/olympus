function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var datalist = document.getElementById("teams");

            var teamArr = JSON.parse(xhr.responseText);
            var i;
            for(i = 0; i  < teamArr.length; i++) {
                var teamOption = document.createElement("option");
                var teamNumber = teamArr[i].number;
                var teamName = teamArr[i].name;

                teamOption.setAttribute("value", teamNumber + " - " + teamName);

                datalist.appendChild(teamOption);
            }
        }
    }
}

function calculateScore(flames,drums,rescue,rescueBonus) {
    var score = 0;
    score += flames * 5;
    if(flames === 30) {
        // "Under Control" Bonus
        score += 30;
    }
    score += drums * 40;
    if(rescue) {
        score += 120;
        score += rescueBonus;
    }
    return score;
}

function submitScore(team,score) {
    var jsonData = {};
    var endOfTeamNumber = team.indexOf(" ");
    jsonData.teamNumber = team.substring(0,endOfTeamNumber);
    alert(jsonData.teamNumber);
    jsonData.score = score;

    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/submit', true);
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            } else {
                alert("Adding team failed. Status: " + xhr.status);
            }
        }
    }
}