var current_top_team = 0;
var teams_per_page = 8;

function getInfo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/active',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var teamArr = JSON.parse(xhr.responseText);

            teamArr.sort(function(a, b){
                var keyA = a.rank;
                var keyB = b.rank;
                if(keyA < keyB) return -1;
                if(keyA > keyB) return 1;
                return 0;
            });

            tabBody = document.getElementsByTagName("tbody").item(0);

            tabBody.innerHTML = "<tr>\n" +
                "        <th scope=\"col\">Rank</th>\n" +
                "        <th scope=\"col\">Team</th>\n" +
                "        <th scope=\"col\">R1</th>\n" +
                "        <th scope=\"col\">R2</th>\n" +
                "        <th scope=\"col\">R3</th>\n" +
                "        <th scope=\"col\">R4</th>\n" +
                "        <th scope=\"col\">R5</th>\n" +
                "        <th scope=\"col\">R6</th>\n" +
                "        <th scope=\"col\">Final</th>\n" +
                "    </tr>";

            var i;
            for(i = current_top_team; i  < Math.min(current_top_team+teams_per_page,teamArr.length); i++) {
                // Populate Table Row
                row=document.createElement("tr");
                rankCell=document.createElement("td");
                teamCell=document.createElement("td");
                r1Cell=document.createElement("td");
                r2Cell=document.createElement("td");
                r3Cell=document.createElement("td");
                r4Cell=document.createElement("td");
                r5Cell=document.createElement("td");
                r6Cell=document.createElement("td");
                scoreCell=document.createElement("td");

                rankCell.appendChild(document.createTextNode(teamArr[i].rank));

                teamCell.appendChild(document.createTextNode(teamArr[i].number + "-" + teamArr[i].name.substr(0,10)));

                if(teamArr[i].scores[0].length > 0)
                    r1Cell.appendChild(document.createTextNode(teamArr[i].scores[0][0]));
                else
                    r1Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 1)
                    r2Cell.appendChild(document.createTextNode(teamArr[i].scores[0][1]));
                else
                    r2Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 2)
                    r3Cell.appendChild(document.createTextNode(teamArr[i].scores[0][2]));
                else
                    r3Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 3)
                    r4Cell.appendChild(document.createTextNode(teamArr[i].scores[0][3]));
                else
                    r4Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 4)
                    r5Cell.appendChild(document.createTextNode(teamArr[i].scores[0][4]));
                else
                    r5Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 5)
                    r6Cell.appendChild(document.createTextNode(teamArr[i].scores[0][5]));
                else
                    r6Cell.appendChild(document.createTextNode("0"));

                scoreCell.appendChild(document.createTextNode(teamArr[i].displayScore));
                row.appendChild(rankCell);
                row.appendChild(teamCell);
                row.appendChild(r1Cell);
                row.appendChild(r2Cell);
                row.appendChild(r3Cell);
                row.appendChild(r4Cell);
                row.appendChild(r5Cell);
                row.appendChild(r6Cell);
                row.appendChild(scoreCell);
                tabBody.appendChild(row);
            }

            current_top_team += teams_per_page;
            if(current_top_team >= teamArr.length) {
                current_top_team = 0;
            }
        }
    };
}

function updateTimer() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET','timer',true);
    xhr.onreadystatechange = ()=> {
        if(xhr.readyState === 4 && xhr.status === 200) {
            let timer_json = JSON.parse(xhr.responseText);
            let time_remaining = timer_json.time_remaining;
            let timer_minutes = Math.floor(time_remaining / 60);
            let timer_seconds = time_remaining % 60;
            document.getElementById('timerDisplay').innerText = timer_minutes.toString() + ":" + timer_seconds.toString().padStart(2,'0');
        }
    }
    xhr.send();
}

function getShowTimer() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'show_timer');
    xhr.send();
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if (xhr.status === 200) {
                if(xhr.responseText === 'false') {
                    document.getElementById('timerDisplay').style.visibility = 'hidden';
                } else {
                    document.getElementById('timerDisplay').style.visibility = 'visible';
                }
            } else {
                alert('Could not get timer config: ' + xhr.responseText);
            }
        }
    }
}

function getTeamsPerPage() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'rows_on_display');
    xhr.send();
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                teams_per_page = parseInt(xhr.responseText);
            } else {
                alert('Could not get rows config: ' + xhr.responseText);
            }
        }
    }
}

function onLoad() {
    getShowTimer();
    getTeamsPerPage();
    getInfo();
    setInterval(getInfo, 5000);
    updateTimer();
    setInterval(updateTimer, 100);
}