const DisplayStates = {
    ShowScores: "ShowScores",
    Blackout: "Blackout",
    FllLogo: "FllLogo",
    Sponsors: "Sponsors"
}

let current_top_team = 0;
let teams_per_page = 8;
let display_state = DisplayStates.ShowScores;
let paging_interval = null;
let seconds_per_page = 5;

function getInfo() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'team/all', true);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var teamArr = JSON.parse(xhr.responseText);

            teamArr.sort(function (a, b) {
                var keyA = a.rank;
                var keyB = b.rank;
                if (keyA < keyB) return -1;
                if (keyA > keyB) return 1;
                return 0;
            });

            let round_count = Math.max(...teamArr.map(team => team.scores.length))

            let tabBody = document.getElementsByTagName("tbody").item(0);
            while (tabBody.firstChild) {
                tabBody.removeChild(tabBody.lastChild);
            }

            let headerRow = document.createElement("tr");

            let rankHeader = document.createElement("th");
            rankHeader.innerText = "Rank";
            rankHeader.attributes["scope"] = "col";
            rankHeader.classList.add("threeStud");
            rankHeader.id = "rankHeader";
            headerRow.appendChild(rankHeader);

            let teamNameHeader = document.createElement("th");
            teamNameHeader.innerText = "Team";
            teamNameHeader.attributes["scope"] = "col";
            teamNameHeader.id = "teamNameHeader";
            headerRow.appendChild(teamNameHeader);

            for (let i = 0; i < round_count; i++) {
                let roundHeader = document.createElement("th");
                roundHeader.innerText = "R" + (i + 1);
                roundHeader.attributes["scope"] = "col";
                roundHeader.classList.add("RoundHeader");
                headerRow.appendChild(roundHeader);
            }

            let finalScoreHeader = document.createElement("th");
            finalScoreHeader.innerText = "Final";
            finalScoreHeader.attributes["scope"] = "col";
            finalScoreHeader.classList.add("threeStud");
            finalScoreHeader.id = "finalScoreHeader";
            headerRow.appendChild(finalScoreHeader);

            tabBody.appendChild(headerRow);

            for (let i = current_top_team; i < Math.min(current_top_team + teams_per_page, teamArr.length); i++) {
                // Populate Table Row
                row = document.createElement("tr");

                rankCell = document.createElement("td");
                rankCell.appendChild(document.createTextNode(teamArr[i].rank));
                row.appendChild(rankCell);

                teamCell = document.createElement("td");
                teamCell.style.whiteSpace = 'pre';
                // teamCell.appendChild(document.createTextNode(teamArr[i].number.padStart(5, ' ') + "  " + teamArr[i].name.substr(0,10)));
                teamCell.appendChild(document.createTextNode(teamArr[i].number.padStart(5, ' ') + "  " + teamArr[i].name));
                row.appendChild(teamCell);

                for (let r = 0; r < round_count; r++) {
                    roundCell = document.createElement("td");
                    if (teamArr[i].scores.length > r) {
                        roundCell.appendChild(document.createTextNode(teamArr[i].scores[r]));
                    } else {
                        roundCell.appendChild(document.createTextNode(" "));
                    }
                    row.appendChild(roundCell);
                }

                scoreCell = document.createElement("td");
                scoreCell.appendChild(document.createTextNode(teamArr[i].displayScore));
                row.appendChild(scoreCell);

                tabBody.appendChild(row);
            }

            current_top_team += teams_per_page;
            if (current_top_team >= teamArr.length) {
                current_top_team = 0;
            }
        }
    };
}

function setContainerVisibility(id, visible) {
    let container = document.querySelector(`.${id}`);
    if(visible === true) {
        container.classList.remove("hidden-container");
    } else {
        container.classList.add("hidden-container");
    }
}

function setScoresTableVisibility(visible) {
    setContainerVisibility("scores-table-container", visible);
}

function setBlackoutVisibility(visible) {
    setContainerVisibility("blackout-container", visible);
}

function setFllLogoVisibility(visible) {
    setContainerVisibility("fll-logo-container", visible);
}

function setSponsorsVisibility(visible) {
    setContainerVisibility("sponsors-container", visible);
}

function getDisplayState() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'display_state');
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                display_state = xhr.responseText;
                switch (display_state) {
                    case DisplayStates.ShowScores:
                        setScoresTableVisibility(true);
                        setBlackoutVisibility(false);
                        setFllLogoVisibility(false);
                        setSponsorsVisibility(false);
                        break;
                    case DisplayStates.Blackout:
                        setScoresTableVisibility(false);
                        setBlackoutVisibility(true);
                        setFllLogoVisibility(false);
                        setSponsorsVisibility(false);
                        break;
                    case DisplayStates.FllLogo:
                        setScoresTableVisibility(false);
                        setBlackoutVisibility(false);
                        setFllLogoVisibility(true);
                        setSponsorsVisibility(false);
                        break;
                    case DisplayStates.Sponsors:
                        setScoresTableVisibility(false);
                        setBlackoutVisibility(false);
                        setFllLogoVisibility(false);
                        setSponsorsVisibility(true);
                        break;
                }
            } else {
                console.error('Could not get display state config: ' + xhr.responseText);
            }
        }
    }
}

function getShowTimer() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'show_timer');
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                if (xhr.responseText === 'false') {
                    document.getElementById('timer-display').style.visibility = 'hidden';
                } else {
                    document.getElementById('timer-display').style.visibility = 'visible';
                }
            } else {
                console.error('Could not get timer config: ' + xhr.responseText);
            }
        }
    }
}

function getTeamsPerPage() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'rows_on_display');
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                teams_per_page = parseInt(xhr.responseText);
                if (document.querySelector(".announcement-container").style.display === "flex") {
                    teams_per_page -= 1;
                }
            } else {
                console.error('Could not get rows config: ' + xhr.responseText);
            }
        }
    }
}

function getSecondsPerPage() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'display_seconds_per_page');
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let new_seconds_per_page = parseInt(xhr.responseText);
                if (new_seconds_per_page !== seconds_per_page) {
                    seconds_per_page = new_seconds_per_page;
                    clearInterval(paging_interval);
                    paging_interval = setInterval(getInfo, seconds_per_page * 1000);
                }
            } else {
                console.error('Could not get seconds per page config: ' + xhr.responseText);
            }
        }
    }
}

function updateDisplayConfig() {
    getDisplayState();
    getShowTimer();
    getTeamsPerPage();
    getSecondsPerPage();
}

function updateAnnouncement() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'announcement', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let announcementDetails = JSON.parse(xhr.responseText);
                let announcementContainer = document.querySelector(".announcement-container");
                let body = document.getElementsByTagName("body")[0];
                if (announcementDetails.visible) {
                    announcementContainer.style.display = "flex";
                    body.style.height = "calc(100% - var(--stud-size))";
                } else {
                    announcementContainer.style.display = "none";
                    body.style.height = "100%";
                }
                document.getElementById("announcement-header").innerText = announcementDetails.content;
            } else {
                console.error('Could not get announcement: ' + xhr.responseText);
            }
        }
    }
}

function updateSponsors() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'sponsors', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let sponsorArr = JSON.parse(xhr.responseText)["sponsors"];
                let sponsorList = document.getElementById("sponsor-list");
                sponsorList.innerText = "";  // clear any existing sponsor images
                for (let i = 0; i < sponsorArr.length; i++) {
                    let sponsorImage = new Image();
                    sponsorImage.src = sponsorArr[i];
                    sponsorImage.className = "sponsor-image";
                    sponsorList.appendChild(sponsorImage);
                }
            } else {
                console.error('Could not get sponsors: ' + xhr.responseText);
            }
        }
    }
}

function onLoad() {
    updateDisplayConfig();
    setInterval(updateDisplayConfig, 1000);
    getInfo();
    paging_interval = setInterval(getInfo, 5000);
    openTimerWebsocket('timer-display');
    updateAnnouncement();
    setInterval(updateAnnouncement, 1000);
    updateSponsors();
    setInterval(updateSponsors, 60000);
}
