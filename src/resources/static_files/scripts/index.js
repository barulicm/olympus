function queryHasTeams() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'controlQuery', true);
    xhr.setRequestHeader('query', 'hasTeams');
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status === 200) {
            if (xhr.responseText === 'true') {
                document.getElementById('teamControls').className += ' disabledcontrol';
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
                document.getElementById('showTimerCheckbox').checked = xhr.responseText === 'true';
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function getDisplaySecondsPerPage() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'display_seconds_per_page');
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                document.getElementById('displaySecondsPerPage').value = parseInt(xhr.responseText);
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function getDisplayState() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'config', true);
    xhr.setRequestHeader('name', 'display_state');
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                document.getElementById('displayState').value = xhr.responseText;
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    };
}

function getActiveGame() {
    let gameSelect = document.getElementById("gameSelect");
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'game/meta', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let gameMetadata = JSON.parse(xhr.responseText);
                gameSelect.value = gameMetadata.name;
            } else if (xhr.status === 204) {
                // No game selected
                let option = document.createElement("option");
                option.innerText = "NOT SELECTED";
                option.setAttribute("value", "");
                gameSelect.appendChild(option);
                gameSelect.value = "";
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function getGames() {
    let gameSelect = document.getElementById('gameSelect');
    gameSelect.innerHTML = '';  // Clears any existing children
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'game/available_games', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let gamesArr = JSON.parse(xhr.responseText);
                for (let i = 0; i < gamesArr.length; i++) {
                    let option = document.createElement("option");
                    option.innerText = gamesArr[i].name + " - " + gamesArr[i].description;
                    option.setAttribute("value", gamesArr[i].name);
                    gameSelect.appendChild(option);
                }
                getActiveGame();
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function sendGameChoice() {
    let chosenGameName = document.getElementById("gameSelect").value;
    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'game/choose', true);
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                location.reload();
            } else {
                alert("Choosing game failed. Status code " + xhr.status + ". " + xhr.responseText);
            }
        }
    }
    xhr.send(chosenGameName);
}

function getActiveTheme() {
    let themeSelect = document.querySelector('#displayTheme');
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'theme/meta', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let gameMetadata = JSON.parse(xhr.responseText);
                themeSelect.value = gameMetadata.name;
            } else if (xhr.status === 204) {
                // No game selected
                let option = document.createElement("option");
                option.innerText = "NOT SELECTED";
                option.setAttribute("value", "");
                themeSelect.appendChild(option);
                themeSelect.value = "";
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function getAvailableThemes() {
    let themeSelect = document.querySelector('#displayTheme');
    themeSelect.innerHTML = '';
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'theme/available_themes', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let themesArr = JSON.parse(xhr.responseText);
                for (let i = 0; i < themesArr.length; i++) {
                    let option = document.createElement("option");
                    option.innerText = themesArr[i].display_name;
                    option.setAttribute("value", themesArr[i].name);
                    themeSelect.appendChild(option);
                }
                getActiveTheme();
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function setDisplayTheme() {
    let chosenThemeName = document.querySelector('#displayTheme').value;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'theme/choose', true);
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                location.reload();
            } else {
                alert("Setting display theme failed. Status code " + xhr.status + ". " + xhr.responseText);
            }
        }
    }
    xhr.send(chosenThemeName);
}

function getSponsors() {
    let sponsorTable = document.getElementById('sponsorTable');
    sponsorTable.innerHTML = '';  // Clears any existing children
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'sponsors', true);
    xhr.send();
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let sponsorArr = JSON.parse(xhr.responseText)["sponsors"];
                for (let i = 0; i < sponsorArr.length; i++) {
                    let row = document.createElement("tr");
                    let imageCell = document.createElement("td");
                    let sponsorImage = new Image();
                    sponsorImage.src = sponsorArr[i];
                    sponsorImage.className = "sponsorImage";
                    imageCell.appendChild(sponsorImage);
                    row.appendChild(imageCell);
                    let delButtonCell = document.createElement("td");
                    let delButton = document.createElement("button");
                    delButton.classList.add("deleteButton");
                    delButton.onclick = () => { deleteSponsor(i); };
                    delButtonCell.appendChild(delButton);
                    row.appendChild(delButtonCell);
                    sponsorTable.appendChild(row);
                }
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function checkForNewRelease() {
    const currentVersionPromise = new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        xhr.open('GET', 'controlQuery', true);
        xhr.setRequestHeader('query', 'version');
        xhr.onload = () => {
            let version = xhr.responseText;
            resolve('v' + version.substring(0, version.indexOf('-')));
        };
        xhr.onerror = reject;
        xhr.send();
    });

    const latestVersionPromise = new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        xhr.open('GET', 'http://api.github.com/repos/barulicm/olympus/releases/latest', true);
        xhr.onload = () => {
            let json_data = JSON.parse(xhr.responseText);
            resolve(json_data['tag_name'])
        };
        xhr.onerror = reject;
        xhr.send();
    });

    Promise.all([currentVersionPromise, latestVersionPromise])
        .then(([currentVersion, latestVersion]) => {
            if (currentVersion < latestVersion) {
                document.getElementById("updateAlert").style.display = 'block';
            }
        })
        .catch(error => {
            console.error("Failed to check for updates: ", error);
        });
}

function onLoad() {
    checkForNewRelease();
    queryHasTeams();
    getShowTimer();
    getDisplaySecondsPerPage();
    getDisplayState();
    getGames();
    getSponsors();
    getAnnouncement();
    getAvailableThemes();
    openTimerWebsocket('timerDisplay', 'endGameAudio', 'endAudio', 'timerButton');
}

function onTeamsFileSelected(e) {
    if (e.target.files.length === 0) {
        location.reload();
    }
    var file = e.target.files[0];
    var reader = new FileReader();
    reader.onload = (function (theFile) {
        return function (e) {
            var contents = e.target.result;
            sendAddTeamsCsv(contents, true, false);
        };
    })(file);

    reader.readAsText(file);
}

function importTeams() {
    let fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.accept = '.csv';
    fileInput.onchange = onTeamsFileSelected;
    fileInput.click();
}

function sendAddTeamsCsv(csv_content, reload, async) {
    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'team/add_csv', async);
    xhr.setRequestHeader("Content-Type", "text/csv");
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                if(reload) {
                    location.reload();
                }
            } else {
                alert("Importing teams failed. Status code " + xhr.status + ". " + xhr.responseText);
            }
        }
    };
    xhr.send(csv_content);
}

function rerankTeamsButtonClicked() {
    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'scores/rerank', true);
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                location.reload();
            } else {
                alert("Reranking teams failed. Status code " + xhr.status + '. ' + xhr.responseText);
            }
        }
    }
    xhr.send();
}

function exportScoresButtonClicked() {
    window.open('scores/export.csv', '_blank');
}

function exportGPScoresButtonClicked() {
    window.open('scores/export_gp.csv', '_blank');
}

function exportSessionButtonClicked() {
    window.open('session_backup.json', '_blank');
}

function importSessionButtonClicked() {
    var fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.accept = '.json';
    fileInput.onchange = e => {
        if (e.target.files.length === 0) {
            location.reload();
        }
        var reader = new FileReader();
        reader.onload = function () {
            let xhr = new XMLHttpRequest();
            xhr.open('PUT', 'session/import', true);
            xhr.setRequestHeader("Content-Type", "application/json");
            xhr.onreadystatechange = () => {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        location.reload();
                    } else {
                        alert("Importing session. Status code " + xhr.status + '. ' + xhr.responseText);
                    }
                }
            };
            xhr.send(reader.result);
        };
        reader.readAsText(e.target.files[0]);
    };
    fileInput.click();
}

function timerButtonClicked() {
    let timer_button = document.getElementById('timerButton');
    if (timer_button.innerText === 'Start Timer') {
        let xhr = new XMLHttpRequest()
        xhr.open('PUT', 'timer/start', true);
        xhr.onreadystatechange = () => {
            if (xhr.readyState === 4) {
                if (xhr.status !== 200) {
                    alert(xhr.responseText);
                } else {
                    document.getElementById('startAudio').play();
                }
            }
        }
        xhr.send();
        timer_button.innerText = 'Stop Timer';
    } else {
        let xhr = new XMLHttpRequest()
        xhr.open('PUT', 'timer/stop', true);
        xhr.onreadystatechange = () => {
            if (xhr.readyState === 4) {
                if (xhr.status !== 200) {
                    alert(xhr.responseText);
                }
            }
        }
        xhr.send();
        if (timer_button.innerText === 'Stop Timer') {
            document.getElementById('stopAudio').play();
        }
        timer_button.innerText = 'Start Timer';
    }
}

function adjustVolumes() {
    let volume = document.getElementById('volumeRange').value / 100.0;
    document.getElementById('startAudio').volume = volume;
    document.getElementById('stopAudio').volume = volume;
    document.getElementById('endGameAudio').volume = volume;
    document.getElementById('endAudio').volume = volume;
}

function setShowTimer() {
    let show_timer = document.getElementById('showTimerCheckbox').checked;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'config', true);
    xhr.setRequestHeader('name', 'show_timer');
    xhr.setRequestHeader('value', show_timer);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
            document.getElementById('showTimerCheckbox').checked = !document.getElementById('showTimerCheckbox').checked;
        }
    }
}

function setDisplaySecondsPerPage() {
    let value = document.getElementById('displaySecondsPerPage').value;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'config', true);
    xhr.setRequestHeader('name', 'display_seconds_per_page');
    xhr.setRequestHeader('value', value);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
        }
    }
}

function setDisplayState() {
    let value = document.getElementById('displayState').value;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'config', true);
    xhr.setRequestHeader('name', 'display_state');
    xhr.setRequestHeader('value', value);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
        }
    }
}

function setAnnouncement() {
    let data = {};
    data.visible = document.getElementById('showAnnouncement').checked;
    data.content = document.getElementById('announcementContent').value;
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'announcement', true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(data));
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4 && xhr.status !== 200) {
            alert('Request failed: ' + xhr.responseText);
        }
    }
}

function getAnnouncement() {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', 'announcement', true);
    xhr.send();

    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                let announcementDetails = JSON.parse(xhr.responseText);
                document.querySelector('#showAnnouncement').checked = announcementDetails.visible;
                document.querySelector('#announcementContent').value = announcementDetails.content;
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    }
}

function addSponsor(element) {
    let file = element.files[0];
    if (file == null) {
        return;
    }
    var reader = new FileReader();
    reader.onloadend = function () {
        let data = {};
        data.image_data = reader.result;
        let xhr = new XMLHttpRequest();
        xhr.open('PUT', 'sponsors/add', true);
        xhr.setRequestHeader("Content-Type", "application/json");
        xhr.send(JSON.stringify(data));
        xhr.onreadystatechange = () => {
            if (xhr.readyState === 4) {
                if (xhr.status === 200) {
                    element.value = '';
                    getSponsors();
                } else {
                    alert('Request failed: ' + xhr.responseText);
                }
            }
        }
    }
    reader.readAsDataURL(file);
}

function deleteSponsor(index) {
    let xhr = new XMLHttpRequest();
    xhr.open('PUT', 'sponsors/delete', true);
    let data = {};
    data['index'] = index;
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(data));
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            if (xhr.status === 200) {
                getSponsors();
            } else {
                alert('Request failed: ' + xhr.responseText);
            }
        }
    };
}
