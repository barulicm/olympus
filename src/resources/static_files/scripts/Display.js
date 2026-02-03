const DisplayStates = {
    ShowScores: "ShowScores",
    Blackout: "Blackout",
    FllLogo: "FllLogo",
    Sponsors: "Sponsors"
}

let config = {
    show_timer: false,
    competition_name: '',
    display_state: DisplayStates.FllLogo,
    display_seconds_per_page: 10,
};

let display_state = DisplayStates.ShowScores;
let paging_interval = null;
let seconds_per_page = 10;

let table_pages = [];
let table_page_index = 0;

let round_count = 0;

function byRank(teamA, teamB) {
    var keyA = teamA.rank;
    var keyB = teamB.rank;
    if (keyA < keyB) return -1;
    if (keyA > keyB) return 1;
    return 0;
}

function updateTablePages() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'team/all', true);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status === 200) {
            var teams = JSON.parse(xhr.responseText);

            round_count = Math.max(...teams.map(t => t.scores.length));

            const tournaments = new Set(teams.map(t => t.tournament));
            const multi_tournament = tournaments.size > 1;

            const header_row_height = parseFloat(window.getComputedStyle(document.querySelector('#scores-table th')).height);
            const table_max_height = parseFloat(window.getComputedStyle(document.querySelector('#scores-table')).maxHeight);
            const header_row_count = multi_tournament ? 2 : 1;
            const page_size = Math.floor(table_max_height / header_row_height) - header_row_count;
        
            table_pages.length = 0;    
            table_page_index = 0;

            let split_function = multi_tournament ? t => t.tournament : t => null;

            const teams_grouped = Object.groupBy(teams, split_function);

            for (const [tournament, group] of Object.entries(teams_grouped)) {
                group.sort(byRank);
                for(let i = 0; i < group.length; i+= page_size) {
                    table_pages.push({
                        tournament: tournament,
                        teams: group.slice(i, i + page_size)
                    });
                }
            }

            renderTablePage();
        }
    };
}

function advanceTablePage() {
    table_page_index++;
    if (table_pages.length == 0 || table_page_index >= table_pages.length) {
        updateTablePages();
        return;
    }
    renderTablePage();
}

function renderTablePage() {
    if (table_page_index >= table_pages.length) {
        return;
    }

    const page = table_pages[table_page_index];

    if(page === null) {
        return;
    }

    const tournament = page.tournament;
    const teams = page.teams;

    let team_number_length = Math.max(...teams.map(team => team.number.length));

    let table_contents = document.querySelector("#table-contents-template").content.cloneNode(true);

    if (tournament !== "null") {
        let tournament_header_row = document.createElement("tr");
        let tournament_header = document.createElement("th");
        tournament_header.appendChild(document.createTextNode("Tournament " + tournament));
        tournament_header.colSpan = 3 + round_count;
        tournament_header.classList.add('tournament-header');
        tournament_header_row.appendChild(tournament_header);
        let header_row = table_contents.querySelector('#header-row');
        table_contents.querySelector('tbody').insertBefore(tournament_header_row, header_row);
    }

    let final_score_header = table_contents.querySelector("#final-score-header");
    for (let i = 0; i < round_count; i++) {
        let round_header = table_contents.querySelector("#round-header-template").content.cloneNode(true);
        round_header.querySelector("th").appendChild(document.createTextNode("R" + (i+1)));
        table_contents.querySelector("#header-row").insertBefore(round_header, final_score_header);
    }

    let final_score_col = table_contents.querySelector(".final-score-col");
    for (let i = 0; i < round_count; i++) {
        let round_col = document.createElement("col");
        round_col.classList.add("round-col");
        table_contents.querySelector("colgroup").insertBefore(round_col, final_score_col);
    }

    for (const team of teams) {
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
        table_contents.querySelector('tbody').appendChild(row);
    }

    document.querySelector("#scores-table").replaceChildren(table_contents);
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

function getConfig() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config/all', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if(xhr.readyState === 4) {
            if (xhr.status === 200) {
                config = JSON.parse(xhr.responseText);
            } else {
                console.error('Could not get display config: ' + xhr.responseText);
            }
        }
    }
}

function updateDisplayState() {
    switch (config.display_state) {
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
}

function updateTimerVisibility() {
    document.querySelector('#timer-display').style.visibility = config.show_timer ? 'visible' : 'hidden';
}

function updatePagingInterval() {
    let new_seconds_per_page = config.display_seconds_per_page;
    if (new_seconds_per_page != seconds_per_page) {
        seconds_per_page = new_seconds_per_page;
        clearInterval(paging_interval);
        paging_interval = setInterval(advanceTablePage, seconds_per_page * 1000);
    }
}

function updateDisplayConfig() {
    getConfig();
    updateDisplayState();
    updateTimerVisibility();
    updatePagingInterval();
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
                let contentContainer = document.querySelector(".content-container");
                let rootContainer = document.querySelector(".root-container");
                if (announcementDetails.visible) {
                    announcementContainer.style.display = "flex";
                    let announcementHeight = parseFloat(window.getComputedStyle(announcementContainer).height);
                    let rootHeight = parseFloat(window.getComputedStyle(rootContainer).height);
                    let contentHeight = rootHeight - announcementHeight;
                    let contentWidth = contentHeight * (16.0 / 9.0);
                    contentContainer.style.height = contentHeight + "px";
                    contentContainer.style.width = contentWidth + "px";
                } else {
                    announcementContainer.style.display = "none";
                    contentContainer.style.height = "100%";
                    contentContainer.style.width = "100%";
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
    advanceTablePage();
    paging_interval = setInterval(advanceTablePage, 5000);
    openTimerWebsocket('timer-display');
    updateAnnouncement();
    setInterval(updateAnnouncement, 1000);
    updateSponsors();
    setInterval(updateSponsors, 60000);
}
