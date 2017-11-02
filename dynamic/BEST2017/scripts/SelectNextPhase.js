function selectNextPhase(phaseResults, teams, currentPhase, nextPhaseMap) {
    if (currentPhase === 0) {
        // Next phase is wildcard, so pick teams based on notebook scores
        teams.sort(function(a, b){
            var keyA = a.customFields.notebookScore;
            var keyB = b.customFields.notebookScore;
            if(keyA < keyB) return  1;
            if(keyA > keyB) return -1;
            return 0;
        });
        var seedRankings = phaseResults.phases[0].rankings;
        seedRankings.sort(function (a, b){
            var keyA = a[0];
            var keyB = b[0];
            if(keyA < keyB) return -1;
            if(keyA > keyB) return 1;
            return 0;
        });
        var top7Teams = [];
        var top7Ind;
        for(top7Ind = 0; top7Ind < top7Teams.length; top7Ind++) {
            top7Teams[top7Ind] = seedRankings[top7Ind];
        }
        var isNotTop7Team = function(t){
            return top7Teams.indexOf(t.number) === -1;
        };
        var wildcardCandidates = teams.filter(isNotTop7Team);
        var mapInd;
        for(mapInd = 0; mapInd < nextPhaseMap.length; mapInd++) {
            var teamKey = nextPhaseMap[mapInd][0];
            var parsedIndex = parseInt(teamKey[9]) - 1;
            nextPhaseMap[mapInd][1] = wildcardCandidates[parsedIndex].number;
        }
    } else {
        // Normal phase transition based on previous phase results
        var seedRankings = phaseResults.phases[0].rankings;
        seedRankings.sort(function (a, b){
            var keyA = a[0];
            var keyB = b[0];
            if(keyA < keyB) return -1;
            if(keyA > keyB) return 1;
            return 0;
        });

        var wildcardRankings = phaseResults.phases[1].rankings;
        wildcardRankings.sort(function (a, b){
            var keyA = a[0];
            var keyB = b[0];
            if(keyA < keyB) return -1;
            if(keyA > keyB) return 1;
            return 0;
        });

        var semifinalsRankings = phaseResults.phases[2].rankings;
        semifinalsRankings.sort(function (a, b){
            var keyA = a[0];
            var keyB = b[0];
            if(keyA < keyB) return -1;
            if(keyA > keyB) return 1;
            return 0;
        });

        var mapInd;
        for(mapInd = 0; mapInd < nextPhaseMap.length; mapInd++) {
            var teamKey = nextPhaseMap[mapInd][0];

            var phaseKey = teamKey.substr(0,teamKey.indexOf(" "));

            var parsedIndex = parseInt(teamKey.substr(teamKey.indexOf(" "))) - 1;

            if(phaseKey === "Seeding") {
                nextPhaseMap[mapInd][1] = seedRankings[parsedIndex][1];
            } else if(phaseKey === "Wildcard") {
                nextPhaseMap[mapInd][1] = wildcardRankings[parsedIndex][1];
            } else if(phaseKey === "Semifinals") {
                nextPhaseMap[mapInd][1] = semifinalsRankings[parsedIndex][1];
            }
        }
    }

    return nextPhaseMap;
}