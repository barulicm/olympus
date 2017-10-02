
Array.min = function(array) {
    return Math.min.apply(Math, array);
};

function add(a, b) {
    return a + b;
}

function GetTeamScore(team)
{
    // TODO FIXME team.scores is an array of arrays of scores b/c phases. Need to also give this function the current phase
    var result = {};
    if(team.scores.length > 1) {
        var average = team.scores.reduce(add, 0);
        average -= Array.min(team.scores);
        average /= (team.scores.length - 1);
        result.score = average;
    } else {
        result.score = team.scores[0];
    }
    return result;
}
