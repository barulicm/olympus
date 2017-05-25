function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState == 4 && xhr.status == 200) {
            var teamArr = JSON.parse(xhr.responseText);
            var i;
            for(i = 0; i  < teamArr.length; i++) {
                // Populate Table Row
                tabBody = document.getElementsByTagName("tbody").item(0);
                row=document.createElement("tr");
                rankCell=document.createElement("td");
                numberCell=document.createElement("td");
                nameCell=document.createElement("td");
                delCell=document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                numberCell.appendChild(document.createTextNode(teamArr[i].number));
                nameCell.appendChild(document.createTextNode(teamArr[i].name));
                delButton = document.createElement("button");
                delButton.classList.add("deleteButton");
                delCell.appendChild(delButton);
                row.appendChild(rankCell);
                row.appendChild(numberCell);
                row.appendChild(nameCell);
                row.appendChild(delCell);
                tabBody.insertBefore(row,tabBody.childNodes[tabBody.childNodes.length-2]);
            }
        }
    };
}

function addTeam() {
    var jsonTeam = {};
    jsonTeam.name = document.getElementsByName("newTeamName")[0].value;
    jsonTeam.number = document.getElementsByName("newTeamNumber")[0].value;
    var jsonString = JSON.stringify(jsonTeam);

    var xhr = new XMLHttpRequest();
    xhr.open('PUT','team/add',true);
    xhr.send(jsonString);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState == 4) {
            if(xhr.status == 200) {
                location.reload();
            } else {
                alert("Adding team failed.");
            }
        }
    };
}

function sortTable(column_number) {
    var table = document.getElementById("teamsTable");
    var switching = true;
    var dir = "asc";
    var switchCount = 0;
    while(switching) {
        switching = false;
        var rows = table.getElementsByTagName("tr");
        for(i=1;i<(rows.length-2);i++) {
            var shouldSwitch = false;
            var x = rows[i].getElementsByTagName("td")[column_number];
            var y = rows[i+1].getElementsByTagName("td")[column_number];
            if(dir == "asc") {
                if(x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {
                    shouldSwitch = true;
                    break;
                }
            } else if(dir == "desc") {
                if(x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {
                    shouldSwitch = true;
                    break;
                }
            }
        }
        if(shouldSwitch) {
            rows[i].parentNode.insertBefore(rows[i+1], rows[i]);
            switching = true;
            switchCount++;
        } else {
            if(switchCount == 0 && dir == "asc") {
                dir = "desc";
                switching = true;
            }
        }
    }
}