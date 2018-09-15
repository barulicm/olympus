function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var teamArr = JSON.parse(xhr.responseText);

            teamArr.sort(function(a, b){
                var keyA = parseInt(a.number);
                var keyB = parseInt(b.number);
                if(keyA < keyB) return -1;
                if(keyA > keyB) return 1;
                return 0;
            });

            var i;
            for(i = 0; i  < teamArr.length; i++) {
                // Populate Table Row
                var tabBody = document.getElementsByTagName("tbody").item(0);
                var row=document.createElement("tr");
                var nameCell=document.createElement("td");
                var scoreCell=document.createElement("td");
                var scoreInput=document.createElement("input");
                scoreInput.setAttribute("type","number");
                scoreInput.setAttribute("step","0.01");

                nameCell.appendChild(document.createTextNode(teamArr[i].number + " - " + teamArr[i].name));

                scoreInput.setAttribute("value",teamArr[i].customFields.notebookScore.toString());

                scoreCell.appendChild(scoreInput);

                row.appendChild(nameCell);
                row.appendChild(scoreCell);
                tabBody.appendChild(row);
            }
        }
    }
}

function setScoreField(teamNumber, score) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/' + teamNumber,true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var team = JSON.parse(xhr.responseText);
            var customFields = team.customFields;

            customFields.notebookScore = score;

            var xhrOutbound = new XMLHttpRequest();
            xhrOutbound.open('PUT','/team/customFields',true);

            var j = {};
            j.teamNumber = teamNumber;
            j.customFields = customFields;
            var jsonString = JSON.stringify(j);

            xhrOutbound.send(jsonString);

            xhrOutbound.onreadystatechange = ()=>{
                if(xhrOutbound.readyState === 4 && xhrOutbound.status !== 200) {
                    alert(xhrOutbound.responseText);
                }
            }

        }
    }
}

function saveButtonClick() {
    var tableRows = document.getElementsByTagName("tr");
    var success = true;
    var rowInd;
    for(rowInd = 1; rowInd < tableRows.length; rowInd++) {
        var nameAndNumber = tableRows[rowInd].children[0].innerText;
        var score = parseFloat(tableRows[rowInd].children[1].children[0].value);
        var n = nameAndNumber.indexOf(" - ");
        var number = nameAndNumber.substr(0,n);
        setScoreField(number, score);
    }
    // location.reload();
}