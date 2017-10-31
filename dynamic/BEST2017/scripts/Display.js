function onLoad() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET','team/active',true);
    xhr.send();

    xhr.onreadystatechange = ()=>{
        if(xhr.readyState === 4 && xhr.status === 200) {
            var teamArr = JSON.parse(xhr.responseText);

            teamArr.sort(function(a, b){
                var keyA = a.rank;
                var keyB = b.rank;
                if(keyA < keyB) return -1;
                if(keyA > keyB) return 1;
                return 0;
            });

            var i;
            for(i = 0; i  < teamArr.length; i++) {
                // Populate Table Row
                tabBody = document.getElementsByTagName("tbody").item(0);
                row=document.createElement("tr");
                rankCell=document.createElement("td");
                numberCell=document.createElement("td");
                nameCell=document.createElement("td");
                scoreCell=document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                numberCell.appendChild(document.createTextNode(teamArr[i].number));
                nameCell.appendChild(document.createTextNode(teamArr[i].name));
                scoreCell.appendChild(document.createTextNode(teamArr[i].displayScore));
                row.appendChild(rankCell);
                row.appendChild(numberCell);
                row.appendChild(nameCell);
                row.appendChild(scoreCell);
                tabBody.appendChild(row);
            }
        }
    };
}