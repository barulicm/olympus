
Array.max = function(array) {
    return Math.max.apply(Math, array);
};

function GetTeamScore(team, currentPhase)
{
    var result = {};

    result.score = Array.max(team.scores[currentPhase]);

    return result;
}
