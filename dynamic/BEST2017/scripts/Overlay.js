function getInfo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','schedule/match/current',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var json = JSON.parse(xhr.responseText);

            var matchNumber = json["number"];

            var matchNumberCell = document.getElementById("matchNumber");
            matchNumberCell.innerHTML = matchNumber;

            var teamArr = json["teams"];

            var redTeamCell = document.getElementById("redTeam");
            var blueTeamCell = document.getElementById("blueTeam");
            var greenTeamCell = document.getElementById("greenTeam");
            var yellowTeamCell = document.getElementById("yellowTeam");

            redTeamCell.innerHTML = teamArr[0];
            blueTeamCell.innerHTML = teamArr[1];
            greenTeamCell.innerHTML = teamArr[2];
            yellowTeamCell.innerHTML = teamArr[3];

        } else if(xhr.readyState === 4) {
            alert("Fail");
        }
    }
}

function onLoad() {
    getInfo();
    setInterval(getInfo, 5000);
}