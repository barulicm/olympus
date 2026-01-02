const DisplayStates = {
    ShowScores: "ShowScores",
    Blackout: "Blackout",
    FllLogo: "FllLogo",
    Sponsors: "Sponsors"
}

let current_top_team = 0;
const teams_per_page = 14;
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

            let round_count = Math.max(...teamArr.map(team => team.scores.length));
            let team_number_length = Math.max(...teamArr.map(team => team.number.length));

            let table_contents = document.querySelector("#table-contents-template").content.cloneNode(true);

            let final_score_header = table_contents.querySelector("#final-score-header");
            for (let i = 0; i < round_count; i++) {
                let round_header = table_contents.querySelector("#round-header-template").content.cloneNode(true);
                round_header.querySelector("th").appendChild(document.createTextNode("R" + (i+1)));
                table_contents.querySelector("#header-row").insertBefore(round_header, final_score_header);
            }

            for (let i = current_top_team; i < Math.min(current_top_team + teams_per_page, teamArr.length); i++) {
                let team = teamArr[i];
                let row = table_contents.querySelector("#score-table-row-template").content.cloneNode(true);
                row.querySelector("#rank-cell").appendChild(document.createTextNode(team.rank));
                row.querySelector("#team-name-cell").appendChild(document.createTextNode(team.number.padStart(team_number_length, ' ') + "  " + team.name));
                let final_score_cell = row.querySelector("#final-score-cell");
                final_score_cell.appendChild(document.createTextNode(team.displayScore));
                for (let r = 0; r < round_count; r++) {
                    let round_cell = document.createElement("td");
                    if (r < team.scores.length) {
                        round_cell.appendChild(document.createTextNode(team.scores[r]));
                    } else {
                        round_cell.appendChild(document.createTextNode(" "));
                    }
                    row.querySelector("tr").insertBefore(round_cell, final_score_cell);
                }
                table_contents.appendChild(row);
            }

            document.querySelector("tbody").replaceChildren(table_contents);

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

function scaleTimerText() {
    
    let timerDisplay = document.querySelector("#timer-display");
    const computedStyle = window.getComputedStyle(timerDisplay);
    const padding = parseFloat(computedStyle.paddingTop) + parseFloat(computedStyle.paddingBottom);
    const fontSize = timerDisplay.clientHeight - padding;
    timerDisplay.style.fontSize = fontSize + "px";
}

function scaleScoresTableText() {

}

function onResize(event) {
    scaleTimerText();
    scaleScoresTableText();
}

function onLoad() {
    onResize();
    addEventListener('resize', onResize);
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
