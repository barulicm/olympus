function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','results',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var json = JSON.parse(xhr.responseText);

            var phaseArr = json["phases"];

            var tabBody = document.getElementsByTagName("tbody").item(0);

            var phaseInd;
            for(phaseInd = 0; phaseInd < phaseArr.length; phaseInd++) {
                var phaseNameRow = document.createElement("tr");
                var phaseNameCell = document.createElement("td");
                phaseNameCell.appendChild(document.createTextNode(phaseArr[phaseInd].name));
                phaseNameCell.setAttribute("colspan","5");
                phaseNameRow.setAttribute("class", "phaseTitle");
                phaseNameRow.appendChild(phaseNameCell);
                tabBody.appendChild(phaseNameRow);

                var rankingsArr = phaseArr[phaseInd].rankings

                rankingsArr.sort(function(a, b){
                    var keyA = a[0];
                    var keyB = b[0];
                    if(keyA < keyB) return -1;
                    if(keyA > keyB) return 1;
                    return 0;
                });

                var rankingsInd;
                for(rankingsInd = 0; rankingsInd < rankingsArr.length; rankingsInd++) {
                    var rankingRow = document.createElement("tr");
                    var rankCell = document.createElement("td");
                    var teamNumberCell = document.createElement("td");

                    rankingRow.setAttribute("class", "rankingRow");

                    rankCell.appendChild(document.createTextNode(rankingsArr[rankingsInd][0]));
                    teamNumberCell.appendChild(document.createTextNode(rankingsArr[rankingsInd][1]));

                    rankingRow.appendChild(rankCell);
                    rankingRow.appendChild(teamNumberCell);
                    tabBody.appendChild(rankingRow);
                }

            }

        } else if(xhr.readyState === 4) {
            alert("Fail");
        }
    }
}
