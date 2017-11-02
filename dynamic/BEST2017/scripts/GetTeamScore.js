
Array.min = function(array) {
    return Math.min.apply(Math, array);
};

function add(a, b) {
    return a + b;
}

function GetTeamScore(team, currentPhase)
{
    var result = {};

    switch(currentPhase) {
        case 0: // Seeding
            if(team.scores[0].length > 1) {
                var average = team.scores[0].reduce(add, 0);
                average -= Array.min(team.scores[0]);
                average /= (team.scores[0].length - 1);
                result.score = average;
            } else if(team.scores[0].length === 1) {
                result.score = team.scores[0][0];
            } else {
                result.score = 0;
            }
            break;

        case 1: // Wildcard
            if(team.scores[1].length > 0) {
                result.score = team.scores[1][0];
            } else {
                result.score = 0;
            }
            break;

        case 2: // Semi-Finals
        case 3: // Finals
            if(team.scores[currentPhase].length > 0) {
                result.score = team.scores[currentPhase].reduce(add, 0);
            } else {
                result.score = 0;
            }
            break;
    }

    return result;
}
