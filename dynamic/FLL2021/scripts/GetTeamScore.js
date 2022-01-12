
Array.max = function(array) {
    return Math.max.apply(Math, array);
};

function GetTeamScore(team, currentPhase)
{
    var result = {};

    if(team.scores[currentPhase].length === 0) {
        result.score = 0;
    } else {
        result.score = Array.max(team.scores[currentPhase]);
    }

    return result;
}
