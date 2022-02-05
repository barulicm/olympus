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

function calculateScore(M00, M01, M02a, M02b, M03a, M03b, M04a, M04b, M05, M06a, M06b, M06c, M07a, M07b, M08a, M08b,
                        M08c, M09a, M09b, M10, M11, M12a, M12b, M13a, M13b, M14, M15a, M15b, M15c, M16a, M16b, M16c,
                        M16d, M16e, M17) {
    let score = 0;

    if(M00 === "y") {
        score += 20;
    }

    if(M01 === "y") {
        score += 20;
    }

    if(M02a === "y") {
        let content_pieces_count = parseInt(M02b);
        if(content_pieces_count > 0) {
            score += 20;
        }
        if(content_pieces_count === 6) {
            score += 10;
        }
    }

    if(M03a === "y") {
        score += 20;
    }
    if(M03b === "y") {
        score += 10;
    }

    if(M04a === "y") {
        score += 10;
    }
    if(M04b === "y") {
        score += 10;
    }
    if(M04a === "y" && M04b === "y") {
        score += 10;
    }

    if(M05 === "y") {
        score += 20;
    }

    if(M06a === "y" && M06c === "n") {
        if(M06b === "y") {
            score += 30;
        } else {
            score += 20;
        }
    }

    if(M07a === "y") {
        score += 20;
        if(M07b === "y") {
            score += 10;
        }
    }

    // TODO only some combinations are valid
    if(M08a === "y") {
        score += 20;
    }
    if(M08b === "y") {
        score += 10;
    }
    if(M08c === "y") {
        score += 10;
    }

    if(M09a === "y") {
        score += 20;
    }
    if(M09b === "y") {
        score += 20;
    }

    if(M10 === "y") {
        score += 20;
    }

    if(M11 === "partly") {
        score += 20;
    }
    else if(M11 === "completely") {
        score += 30;
    }

    if(M12a === "mat") {
        score += 20;
    }
    else if(M12a === "nothing") {
        score += 30;
    }
    if(M12b === "partly") {
        score += 5;
    }
    else if(M12b === "completely") {
        score += 10;
    }

    if(M13a === "y") {
        score += 10;
    }
    if(M13b === "y") {
        score += 10;
    }
    if(M13a === "y" && M13b === "y") {
        score += 10;
    }

    score += parseInt(M14) * 10;

    score += parseInt(M15a) * 10;
    score += parseInt(M15b) * 20;
    score += parseInt(M15c) * 30;

    // TODO only some combinations are valid
    score += parseInt(M16a) * 5;
    score += parseInt(M16b) * 10;
    if(parseInt(M16b) > 0) {
        if(M16c === "y") {
            score += 20;
        }
        if(M16d === "y") {
            score += 20;
        }
        score += parseInt(M16e) * 10;
    }

    if(M17 === "1") {
        score += 10;
    } else if(M17 === "2") {
        score += 15;
    } else if(M17 === "3") {
        score += 25;
    } else if(M17 === "4") {
        score += 35;
    } else if(M17 === "5") {
        score += 50;
    } else if(M17 === "6") {
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
                location.reload();
            } else {
                alert("Submitting score failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}