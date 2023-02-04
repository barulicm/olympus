
function addTableHeaders(tableBody, roundCount) {
    let headerRow = document.createElement("tr");
    let rankHeader = document.createElement("th");
    rankHeader.appendChild(document.createTextNode("Rank"));
    headerRow.appendChild(rankHeader);
    let teamHeader = document.createElement("th");
    teamHeader.appendChild(document.createTextNode("Team"));
    headerRow.appendChild(teamHeader);
    for( let i=0; i < roundCount; i++) {
        let roundHeader = document.createElement("th");
        roundHeader.appendChild(document.createTextNode("Round " + (i+1)));
        headerRow.appendChild(roundHeader);
    }
    let finalScoreHeader = document.createElement("th");
    finalScoreHeader.appendChild(document.createTextNode("Final Score"));
    headerRow.appendChild(finalScoreHeader);
    tableBody.appendChild(headerRow);
}

function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            let teams = JSON.parse(xhr.responseText);

            let tableBody = document.getElementsByTagName("tbody").item(0);

            let roundCount = Math.max(... teams.map(team => team.scores.length))

            addTableHeaders(tableBody, roundCount);

            for(let team of teams) {
                let teamRow = document.createElement("tr");
                teamRow.setAttribute("class", "rankingRow");
                let rankCell = document.createElement("td");
                rankCell.appendChild(document.createTextNode(team.rank));
                teamRow.appendChild(rankCell);
                let teamCell = document.createElement("td");
                teamCell.appendChild(document.createTextNode(team.number));
                teamRow.appendChild(teamCell);
                for(let score of team.scores) {
                    let scoreCell = document.createElement("td");
                    scoreCell.appendChild(document.createTextNode(score));
                    teamRow.appendChild(scoreCell);
                }
                for(let i = 0; i < (roundCount - team.scores.length); i++) {
                    let scoreCell = document.createElement("td");
                    // scoreCell intentionally left blank
                    teamRow.appendChild(scoreCell);
                }
                let finalScoreCell = document.createElement("td");
                finalScoreCell.appendChild(document.createTextNode(team.displayScore));
                teamRow.appendChild(finalScoreCell);
                tableBody.appendChild(teamRow);
            }

        } else if(xhr.readyState === 4) {
            alert("Fail");
        }
    }
}
