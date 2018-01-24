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

function calculateScore(M01, M02, M03, M04, M05, M06, M07, M08a, M08b, M09a, M09b, M09c, M10a, M10b, M11a, M11b, M11c,
                        M12, M13a, M13b, M14a, M14b, M15, M16a, M16b, M16c, M16d, M17a, M17b, M18, Penalties) {
    var score = 0;

    if(M01 === "y") {
        score += 20;
    }

    if(M02 === "y") {
        score += 25;
    }

    if(M03 === "y") {
        score += 20;
    }

    if(M04 === "y") {
        score += 20;
    }

    if(M05 === "y") {
        score += 30;
    }

    if(M06 === "y") {
        score += 20;
    }

    if(M07 === "y") {
        score += 20;
    }

    score += parseInt(M08a) * 15;
    if(M08b === "y") {
        score += 30;
    }

    if(M09a === "y") {
        if(M09b === "y") {
            score += 15;
        }
        else if(M09c === "y") {
            score += 20;
        }
    }

    if(M10a === "y" && M10b === "y") {
        score += 20;
    }

    if(M11a === "y") {
        if(M11b === "y") {
            score += 15;
        }
        if(M11c === "y") {
            score += 20;
        }
    }

    if(M12 === "y") {
        score += 30;
    }

    if(M13a === "y") {
        score += 30;
        if(M13b === "y") {
            score += 30;
        }
    }

    if(M14a === "y") {
        score += 15;
    }
    else if(M14b === "y") {
        score += 25;
    }

    if(M15 === "y") {
        score += 25;
    }

    if(M16a === "y") {
        if(M16b === "y") {
            score += 10;
        }
        score += parseInt(M16c) * 10;
        if(M16d === "y") {
            score += 30;
        }
    }

    if(M17a === "y") {
        score += 20;
        if(M17b === "y") {
            score += 15;
        }
    }


    if(M18 === "y") {
        score += 25;
    }

    score += parseInt(Penalties) * -5;

    return score;
}

function submitScore(team,score) {
    alert(score)
    var jsonData = {};
    var endOfTeamNumber = team.indexOf(" ");
    jsonData.teamNumber = team.substring(0,endOfTeamNumber);
    jsonData.score = score;

    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/submit', true);
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            } else {
                alert("Submitting score failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}