
Array.min = function(array) {
    return Math.min.apply(Math, array);
};

function add(a, b) {
    return a + b;
}

function GetTeamScore(team)
{
    var result = {};
    var average = team.scores.reduce(add, 0);
    average -= Array.min(team.scores);
    average /= (team.scores.length - 1);
    result.score = average;
    return result;
}
