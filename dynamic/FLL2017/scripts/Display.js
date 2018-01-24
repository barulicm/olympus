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
                "        <th scope=\"col\">#</th>\n" +
                "        <th scope=\"col\">Name</th>\n" +
                "        <th scope=\"col\">M1</th>\n" +
                "        <th scope=\"col\">M2</th>\n" +
                "        <th scope=\"col\">M3</th>\n" +
                "        <th scope=\"col\">Final</th>\n" +
                "    </tr>";

            var i;
            for(i = current_top_team; i  < Math.min(current_top_team+teams_per_page,teamArr.length); i++) {
                // Populate Table Row
                row=document.createElement("tr");
                rankCell=document.createElement("td");
                numberCell=document.createElement("td");
                numberCell.className += ' numberCell';
                nameCell=document.createElement("td");
                m1Cell=document.createElement("td");
                m2Cell=document.createElement("td");
                m3Cell=document.createElement("td");
                scoreCell=document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                numberCell.appendChild(document.createTextNode(teamArr[i].number));
                nameCell.appendChild(document.createTextNode(teamArr[i].name.substr(0,26)));

                if(teamArr[i].scores[0].length > 0)
                    m1Cell.appendChild(document.createTextNode(teamArr[i].scores[0][0]));
                else
                    m1Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 1)
                    m2Cell.appendChild(document.createTextNode(teamArr[i].scores[0][1]));
                else
                    m2Cell.appendChild(document.createTextNode("0"));

                if(teamArr[i].scores[0].length > 2)
                    m3Cell.appendChild(document.createTextNode(teamArr[i].scores[0][2]));
                else
                    m3Cell.appendChild(document.createTextNode("0"));

                scoreCell.appendChild(document.createTextNode(teamArr[i].displayScore));
                row.appendChild(rankCell);
                row.appendChild(numberCell);
                row.appendChild(nameCell);
                row.appendChild(m1Cell);
                row.appendChild(m2Cell);
                row.appendChild(m3Cell);
                row.appendChild(scoreCell);
                tabBody.appendChild(row);
            }

            current_top_team += teams_per_page;
            if(current_top_team > teamArr.length) {
                current_top_team = 0;
            }
        }
    };
}

function onLoad() {
    getInfo();
    setInterval(getInfo, 5000);
}