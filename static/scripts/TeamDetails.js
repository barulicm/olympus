function onLoad() {
    var current_url = window.location.href;
    var team_number = current_url.substr(current_url.indexOf("?team=")+6);

    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/'+team_numer,true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState == 4 && xhr.status == 200) {
            var team = JSON.parse(xhr.responseText);


            // var teamArr = JSON.parse(xhr.responseText);
            // var i;
            // for(i = 0; i  < teamArr.length; i++) {
            //     // Populate Table Row
            //     tabBody = document.getElementsByTagName("tbody").item(0);
            //     row=document.createElement("tr");
            //     rankCell=document.createElement("td");
            //     numberCell=document.createElement("td");
            //     nameCell=document.createElement("td");
            //     delCell=document.createElement("td");
            //     rankCell.appendChild(document.createTextNode(teamArr[i].rank));
            //     numberCell.appendChild(document.createTextNode(teamArr[i].number));
            //     var teamNameLink = document.createElement("a");
            //     teamNameLink.setAttribute("href", 'TeamDetails.html?team=' + teamArr[i].number);
            //     teamNameLink.innerText = teamArr[i].name;
            //     nameCell.appendChild(teamNameLink);
            //     delButton = document.createElement("button");
            //     delButton.classList.add("deleteButton");
            //     delButton.id = teamArr[i].number;
            //     delButton.onclick = removeTeam;
            //     delCell.appendChild(delButton);
            //     row.appendChild(rankCell);
            //     row.appendChild(numberCell);
            //     row.appendChild(nameCell);
            //     row.appendChild(delCell);
            //     tabBody.insertBefore(row,tabBody.childNodes[tabBody.childNodes.length-2]);
            // }
        }
    }
}