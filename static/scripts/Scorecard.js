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

function calculateScore(M00, M01, M02a, M02b, M03a, M03b, M04, M05a, M05b, M06a, M06b, M07, M08a, M08b, M09a, M09b,
                        M10, M11, M12a, M12b, M13, M14a, M14b, M15, M16) {
    let score = 0;

    if(M00 === "y") {
        score += 20;
    }

    if(M01 === "y") {
        score += 10;
    }

    score += 5 * parseInt(M02a);
    if(parseInt(M02a) > 0 && M02b === "y") {
        score += 10;
    }

    score += 10 * parseInt(M03a);
    if(M03b === "y") {
        score += 5;
    }

    let m04_energy_units_count = parseInt(M04);
    score += 5 * m04_energy_units_count;
    if(m04_energy_units_count === 3) {
        score += 5;
    }

    // TODO: M05a=n & M05b=y is invalid ("Cannot have both team's orange connectors raised, but not the team's connector raised")
    if(M05a === "y") {
        score += 20;
    }
    if(M05b === "y") {
        score += 10;
    }

    if(M06a === "y") {
        score += 10;
    }
    if(M06b === "y") {
        score += 10
    }

    score += 10 * parseInt(M07);

    if(M08a === "y") {
        score += 10;
    }
    if(M08b === "y") {
        score += 10;
    }

    if(M09a === "y") {
        score += 10;
    }
    if(M09b === "energy_unit") {
        score += 10;
    }
    else if(M09b === "rechargeable_battery") {
        score += 20;
    }

    let m10_energy_units = parseInt(M10);
    score += 5 * m10_energy_units;
    if(m10_energy_units === 3) {
        score += 10;
    }

    if(M11 === "y") {
        score += 20;
    }

    // TODO: Only some combinations are valid
    score += 5 * parseInt(M12a);
    score += 10 * parseInt(M12b);

    score += 5 * parseInt(M13);

    score += 5 * parseInt(M14a);
    if(M14b === "y") {
        score += 10;
    }

    score += 5 * parseInt(M15);

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

function submitScore(team,score) {
    let jsonData = {};
    let endOfTeamNumber = team.indexOf(" ");
    jsonData.teamNumber = team.substring(0,endOfTeamNumber);
    jsonData.score = score;

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