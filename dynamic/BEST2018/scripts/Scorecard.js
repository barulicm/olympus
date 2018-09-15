function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/all',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var datalist = document.getElementById("teams");

            var teamArr = JSON.parse(xhr.responseText);
            var i;
            for(i = 0; i  < teamArr.length; i++) {
                var teamOption = document.createElement("option");
                var teamNumber = teamArr[i].number;
                var teamName = teamArr[i].name;

                teamOption.setAttribute("value", teamNumber + " - " + teamName);

                datalist.appendChild(teamOption);
            }
        }
    }
}

function calculateScore(formdata) {
    var score = 0;

    score += formdata.cans * 10;
    score += formdata.sbottles * 10;
    score += formdata.mbottles * 10;
    score += formdata.lbottles * 10;

    score += formdata.can_blocks * 65;
    score += formdata.sbottle_blocks * 65;
    score += formdata.mbottle_blocks * 65;
    score += formdata.lbottle_blocks * 65;

    score += formdata.can_blocks_installed * 165;
    score += formdata.sbottle_blocks_installed * 165;
    score += formdata.mbottle_blocks_installed * 165;
    score += formdata.lbottle_blocks_installed * 165;

    score += formdata.microplastics * 5;
    score += formdata.tubes * 90;
    score += formdata.tubes_installed * 190;




    return score;
}

function submitScore(team,score) {
    var jsonData = {};
    var endOfTeamNumber = team.indexOf(" ");
    jsonData.teamNumber = team.substring(0,endOfTeamNumber);
    jsonData.score = score;

    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/submit', true);
    xhr.send(JSON.stringify(jsonData));
    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4) {
            if(xhr.status === 200) {
                location.reload();
            } else {
                alert("Submitting score failed.\nStatus: " + xhr.status + "\nMessage: " + xhr.responseText);
            }
        }
    }
}