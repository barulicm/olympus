function onLoad() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            let teamArr = JSON.parse(xhr.responseText);
            let i;
            for(i = 0; i  < teamArr.length; i++) {
                // Populate Table Row
                let tabBody = document.getElementsByTagName("tbody").item(0);
                let row=document.createElement("tr");
                let rankCell=document.createElement("td");
                let numberCell=document.createElement("td");
                let nameCell=document.createElement("td");
                let buttonsCell=document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                numberCell.appendChild(document.createTextNode(teamArr[i].number));
                let teamNameLink = document.createElement("a");
                let teamDetailsUrl = 'TeamDetails.html?team=' + teamArr[i].number;
                teamNameLink.setAttribute("href", teamDetailsUrl);
                teamNameLink.innerText = teamArr[i].name;
                nameCell.appendChild(teamNameLink);
                let delButton = document.createElement("button");
                delButton.classList.add("deleteButton");
                delButton.id = teamArr[i].number;
                delButton.onclick = removeTeam;
                buttonsCell.appendChild(delButton);
                let editButton = document.createElement("button");
                editButton.classList.add("editButton");
                editButton.onclick = () => { window.open(teamDetailsUrl, "_self") };
                buttonsCell.appendChild(editButton);
                row.appendChild(rankCell);
                row.appendChild(numberCell);
                row.appendChild(nameCell);
                row.appendChild(buttonsCell);
                tabBody.insertBefore(row,tabBody.childNodes[tabBody.childNodes.length-2]);
            }
        }
    }
}

function removeTeam(evt) {
    let teamNumber = evt.target.id;
    let jsonData = {};
    jsonData.number = teamNumber;
    let jsonString = JSON.stringify(jsonData);

    let xhr = new XMLHttpRequest();
    xhr.open('PUT','team/remove',true);
    xhr.setRequestHeader("Content-Type", "application/json");
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
    let jsonTeam = {};
    jsonTeam.name = teamName;
    jsonTeam.number = teamNumber;
    let jsonString = JSON.stringify(jsonTeam);

    let xhr = new XMLHttpRequest();
    xhr.open('PUT','team/add',async);
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                if (reload) {
                    location.reload();
                }
            } else {
                alert("Adding team " + teamNumber + " failed. " + xhr.responseText + ' (' + xhr.status + ')');
            }
        }
    }
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(jsonString);
}

function sortTable(column_number) {
    let table = document.getElementById("teamsTable");
    let switching = true;
    let dir = "asc";
    let switchCount = 0;
    let asNumeric = (column_number === 0 || column_number === 1);
    while(switching) {
        switching = false;
        let rows = table.getElementsByTagName("tr");
        let shouldSwitch = false;
        for(i=1;i<(rows.length-2);i++) {
            shouldSwitch = false;
            let x = rows[i].getElementsByTagName("td")[column_number];
            let y = rows[i+1].getElementsByTagName("td")[column_number];
            let x_val = x.innerText.toLowerCase();
            let y_val = y.innerText.toLowerCase();
            if(asNumeric) {
                x_val = parseInt(x_val);
                y_val = parseInt(y_val);
            }
            if(dir === "asc") {
                if(x_val > y_val) {
                    shouldSwitch = true;
                    break;
                }
            } else if(dir === "desc") {
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
            if(switchCount === 0 && dir === "asc") {
                dir = "desc";
                switching = true;
            }
        }
    }
}
