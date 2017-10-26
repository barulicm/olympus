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

            var phaseSelect = document.getElementById("currentPhase");

            var matchSelect = document.getElementById("currentMatch");

            for(phaseInd = 0; phaseInd < phaseArr.length; phaseInd++) {
                var phaseNameRow = document.createElement("tr");
                var phaseNameCell = document.createElement("td");
                phaseNameCell.appendChild(document.createTextNode(phaseArr[phaseInd].name));
                phaseNameCell.setAttribute("colspan","5");
                phaseNameRow.setAttribute("class", "phaseTitle");
                phaseNameRow.appendChild(phaseNameCell);

                var phaseOption = document.createElement("option");
                phaseOption.setAttribute("value",phaseArr[phaseInd].name);
                phaseOption.appendChild(document.createTextNode(phaseArr[phaseInd].name));
                phaseSelect.appendChild(phaseOption);

                tabBody.appendChild(phaseNameRow);
                var matchArr = phaseArr[phaseInd]["matches"];
                var matchInd;
                for(matchInd = 0; matchInd < matchArr.length; matchInd++) {
                    var matchRow = document.createElement("tr");
                    if(matchInd === currentMatchIndex) {
                        matchRow.setAttribute("class","currentMatchClass");
                    }
                    var matchNumberCell = document.createElement("td");
                    matchNumberCell.setAttribute("class","matchNumber");
                    matchNumberCell.appendChild(document.createTextNode(matchArr[matchInd]["number"]));
                    matchRow.appendChild(matchNumberCell);

                    if(phaseInd === currentPhaseIndex) {
                        var matchOption = document.createElement("option");
                        matchOption.setAttribute("value", matchArr[matchInd].number);
                        matchOption.appendChild(document.createTextNode("Match " + matchArr[matchInd].number));
                        matchSelect.appendChild(matchOption);
                    }

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

            phaseSelect.selectedIndex = currentPhaseIndex;
            matchSelect.selectedIndex = currentMatchIndex;

        } else if(xhr.readyState === 4) {
            alert("Fail");
        }
    }
}

function importSchedule() {
    var files = document.getElementById('importFile').files;
    if(files.length === 0) {
        alert("Please select a file first.");
        return;
    }
    var file = files[0];
    var reader = new FileReader();
    reader.onload = (function(theFile) {
        return function(e) {
            var xhr = new XMLHttpRequest();
            xhr.open('PUT','schedule/load',true);
            xhr.onreadystatechange = ()=>{
                if(xhr.readyState === 4) {
                    if(xhr.status === 200) {
                        location.reload();
                    } else {
                        alert("Loading schedule failed.\nStatus code: " + xhr.status + "\nResponse: " + xhr.responseText);
                    }
                }
            }
            xhr.send(e.target.result);
        };
    })(file);

    reader.readAsText(file);
}
