function onLoad() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            let teamArr = JSON.parse(xhr.responseText);
            let row_template = document.getElementById("rowTemplate");
            let tab_body = document.getElementsByTagName("tbody").item(0);
            for(let i = 0; i < teamArr.length; i++) {
                let team = teamArr[i];
                let teamDetailsUrl = 'team_details.html?team=' + team.number;
                let row = document.importNode(row_template.content, true);
                row.querySelector("#rank").appendChild(document.createTextNode(team.rank));
                row.querySelector("#number").appendChild(document.createTextNode(team.number));
                let team_name_link = row.querySelector("#nameLink");
                team_name_link.href = teamDetailsUrl;
                team_name_link.appendChild(document.createTextNode(team.name));
                row.querySelector(".editButton").onclick = () => { window.open(teamDetailsUrl, "_self")};
                tab_body.insertBefore(row,tab_body.childNodes[tab_body.childNodes.length-2]);
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
