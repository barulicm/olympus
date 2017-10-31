function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','schedule/full',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var json = JSON.parse(xhr.responseText);

            var currentPhaseIndex = json["currentPhase"];
            var currentMatchIndex = json["currentMatch"];

            var phaseArr = json["phases"];
            var phaseInd;
            var tabBody = document.getElementsByTagName("tbody").item(0);

            for(phaseInd = 0; phaseInd < phaseArr.length; phaseInd++) {
                var phaseNameRow = document.createElement("tr");
                var phaseNameCell = document.createElement("td");
                phaseNameCell.appendChild(document.createTextNode(phaseArr[phaseInd].name));
                phaseNameCell.setAttribute("colspan","5");
                phaseNameRow.setAttribute("class", "phaseTitle");
                phaseNameRow.appendChild(phaseNameCell);

                tabBody.appendChild(phaseNameRow);
                var matchArr = phaseArr[phaseInd]["matches"];
                var matchInd;
                for(matchInd = 0; matchInd < matchArr.length; matchInd++) {
                    var matchRow = document.createElement("tr");
                    if(phaseInd === currentPhaseIndex && matchInd === currentMatchIndex) {
                        matchRow.setAttribute("class","currentMatchClass");
                    }
                    var matchNumberCell = document.createElement("td");
                    matchNumberCell.setAttribute("class","matchNumber");
                    matchNumberCell.appendChild(document.createTextNode(matchArr[matchInd]["number"]));
                    matchRow.appendChild(matchNumberCell);

                    var teamArr = matchArr[matchInd]["teams"];
                    var teamInd;
                    for(teamInd = 0; teamInd < teamArr.length; teamInd++) {
                        var teamCell = document.createElement("td");
                        teamCell.appendChild(document.createTextNode(teamArr[teamInd]));
                        matchRow.appendChild(teamCell);
                    }
                    tabBody.appendChild(matchRow);
                }
            }

        } else if(xhr.readyState === 4) {
            alert("Fail");
        }
    }
}
