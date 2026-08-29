function calculateScore(M0a,M1a,M2a)
{
  let score = 0;

  
  {
    
    
    let qa = M0a === 'y';
    
    
    
    
    score += (qa ? 20 : 0);
  }
  
  {
    
    
    
    
    let qa = parseInt(M1a);
    
    
    score += (qa * 5);
  }
  
  {
    
    
    
    let qa = M2a;
    
    
    
    score += (qa == "Option A" ? 10 : 0);
  }
  

  return score;
}

function validateScorecard(M0a,M1a,M2a)
{
  let validity = {};

  
  {
    
    
    let qa = M0a === 'y';
    
    
    
    
    validity["M0"] = Boolean(true);
  }
  
  {
    
    
    
    
    let qa = parseInt(M1a);
    
    
    validity["M1"] = Boolean(qa <= 2);
  }
  
  {
    
    
    
    let qa = M2a;
    
    
    
    validity["M2"] = Boolean(qa != "Option C");
  }
  

  return validity;
}

function onScorecardChange()
{
  let form = document.getElementById("scorecardForm");
  let questionValues = [form.elements["M0a"].value, form.elements["M1a"].value, form.elements["M2a"].value];

  document.getElementById("scoreOutput").value = calculateScore(...questionValues);

  let validity = validateScorecard(...questionValues);
  let allValid = true;
  for (let missionId in validity) {
    let block = document.getElementById(missionId + "Block");
    if (validity[missionId]) {
      block.classList.remove("invalid-mission");
    } else {
      block.classList.add("invalid-mission");
      allValid = false;
    }
  }
  document.getElementById("submitButton").disabled = !allValid;
}
