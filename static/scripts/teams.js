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
                var teamNameLink = document.createElement("a");
                teamNameLink.setAttribute("href", 'TeamDetails.html?team=' + teamArr[i].number);
                teamNameLink.innerText = teamArr[i].name;
                nameCell.appendChild(teamNameLink);
                delButton = document.createElement("button");
                delButton.classList.add("deleteButton");
                delButton.id = teamArr[i].number;
                delButton.onclick = removeTeam;
                delCell.appendChild(delButton);
                row.appendChild(rankCell);
                row.appendChild(numberCell);
                row.appendChild(nameCell);
                row.appendChild(delCell);
                tabBody.insertBefore(row,tabBody.childNodes[tabBody.childNodes.length-2]);
            }
        }
    }
}

function removeTeam(evt) {
    var teamNumber = evt.target.id;
    var jsonData = {};
    jsonData.number = teamNumber;
    var jsonString = JSON.stringify(jsonData);

    var xhr = new XMLHttpRequest();
    xhr.open('PUT','team/remove',true);
    xhr.send(jsonString);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            } else {
                alert("Removing team " + teamNumber + " failed.");
            }
        }
    }
}

function addTeam() {
    sendAddTeam(document.getElementsByName("newTeamName")[0].value,document.getElementsByName("newTeamNumber")[0].value, true, true);
}

function sendAddTeam(teamName, teamNumber, reload, async) {
    var jsonTeam = {};
    jsonTeam.name = teamName;
    jsonTeam.number = teamNumber;
    var jsonString = JSON.stringify(jsonTeam);

    var xhr = new XMLHttpRequest();
    xhr.open('PUT','team/add',async);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                if (reload) {
                    location.reload();
                }
            } else {
                alert("Adding team " + teamNumber + " failed. Status code " + xhr.status);
            }
        }
    }
    xhr.send(jsonString);
}

function sortTable(column_number) {
    var table = document.getElementById("teamsTable");
    var switching = true;
    var dir = "asc";
    var switchCount = 0;
    var asNumeric = (column_number === 0 || column_number === 1);
    while(switching) {
        switching = false;
        var rows = table.getElementsByTagName("tr");
        for(i=1;i<(rows.length-2);i++) {
            var shouldSwitch = false;
            var x = rows[i].getElementsByTagName("td")[column_number];
            var y = rows[i+1].getElementsByTagName("td")[column_number];
            var x_val = x.innerHTML.toLowerCase();
            var y_val = y.innerHTML.toLowerCase();
            if(asNumeric) {
                x_val = parseInt(x_val);
                y_val = parseInt(y_val);
            }
            if(dir == "asc") {
                if(x_val > y_val) {
                    shouldSwitch = true;
                    break;
                }
            } else if(dir == "desc") {
                if(x_val < y_val) {
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
