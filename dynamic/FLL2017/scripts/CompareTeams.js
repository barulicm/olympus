function CompareTeams(a, b)
{
    var result = {};
    var scoreA = a.displayScore;
    var scoreB = b.displayScore;
    result.result = (scoreA > scoreB);
    return result;
}
