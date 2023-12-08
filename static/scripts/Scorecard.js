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

function calculateScore(M00, M01, M02a, M02b, M03, M04a, M04b, M05, M06a, M06b, M07, M08, M09a, M09b,
                        M10, M11, M12a, M12b, M13a, M13b, M14a, M14b, M15, M16) {
    let score = 0;

    if(M00 === "y") {
        score += 20;
    }

    if(M01 === "y") {
        score += 20;
    }

    if(M02a === "blue") {
        score += 10;
        if(M02b === "y") {
            score += 20;
        }
    }
    else if(M02a === "pink") {
        score += 20;
        if(M02b === "y") {
            score += 30;
        }
    }
    else if(M02a === "orange") {
        score += 30;
        if(M02b === "y") {
            score += 10;
        }
    }

    if(M03 === "y") {
        score += 20;
    }

    if(M04a === "y") {
        score += 10;
        if(M04b === "y") {
            score += 20;
        }
    }

    if(M05 === "y") {
        score += 30;
    }

    if(M06a === "y") {
        score += 10;
    }
    if(M06b === "y") {
        score += 10;
    }

    if(M07 === "y") {
        score += 20;
    }

    if(M08 === "1") {
        score += 10;
    }
    else if(M08 === "2") {
        score += 20;
    }
    else if(M08 === "3") {
        score += 30;
    }

    if(M09a === "y") {
        score += 10;
    }
    if(M09b === "y") {
        score += 10;
    }

    score += 10 * parseInt(M10);

    score += 10 * parseInt(M11);

    if(M12a === "y") {
        score += 10;
        if(M12b === "y") {
            score += 20;
        }
    }

    if(M13a === "y") {
        score += 10;
    }
    if(M13b === "y") {
        score += 20;
    }

    score += 5 * parseInt(M14a);
    score += 5 * parseInt(M14b);

    score += 10 * parseInt(M15);

    if(M16 === "1") {
        score += 10;
    } else if(M16 === "2") {
        score += 15;
    } else if(M16 === "3") {
        score += 25;
    } else if(M16 === "4") {
        score += 35;
    } else if(M16 === "5") {
        score += 50;
    } else if(M16 === "6") {
        score += 50;
    }

    return score;
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